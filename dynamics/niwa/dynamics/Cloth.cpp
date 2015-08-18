/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "Cloth.h"

#include "ClothCollider.h"

#include "niwa/levelset/ILevelSet.h"

#include "niwa/math/blas.h"
#include "niwa/math/ConjugateGradient.h"

#include <algorithm>

#define GRAVITY 9.81f

#define USE_SSE

//#define USE_SSE_STRAIN_LIMITING

namespace niwa {
    namespace dynamics {
        using namespace math::blas;

        using math::vec3f;

        Cloth::Cloth(
                std::vector<ClothParticle> const& particles,
                std::vector<ClothSpring> const& springs,
                std::vector<Triangle> const& triangles,
                float thickness)
            : particles_(particles), springs_(springs),
              triangles_(triangles),
              useImplicitMethod_(false),
              useStrainLimiting_(false),
              thickness_(thickness) {
            std::sort(springs_.begin(), springs_.end());

            int nPackedSprings = springs_.size() / 4;

            packedSprings_ = new ClothPackedSpring[nPackedSprings];

            std::vector<ClothSpring>::const_iterator it = springs.begin();

            for(int i=0; i<nPackedSprings; ++i, it += 4) {
                packedSprings_[i].setup(it);
            }
        }

        Cloth::~Cloth() {
            delete[] packedSprings_;
        }

        struct ImplicitOperatorArg {
            ImplicitOperatorArg(Cloth const* cloth_, float timeSeconds_)
                : cloth(cloth_), timeSeconds(timeSeconds_) {
                // ignored
            }

            Cloth const* cloth;
            float timeSeconds;
        };

        void Cloth::implicitOperator(
                Vector const& src, Vector& dst, void* arg) {
            ImplicitOperatorArg* ioa = 
                static_cast<ImplicitOperatorArg*>(arg);

            ioa->cloth->implicitOperator(src, dst, ioa->timeSeconds);
        }

        void Cloth::implicitOperator(
                Vector const& src, Vector& dst, float timeSeconds) const {
            int n = particles_.size();

            if(useImplicitMethod_) {
                szero(3*n, dst);

                vec3f const* x = reinterpret_cast<vec3f const*>(src.getRaw());
                vec3f* y = reinterpret_cast<vec3f*>(dst.getRaw());

#ifdef USE_SSE
                size_t nPackedSprings = springs_.size() / 4;
#else
                size_t nPackedSprings = 0;
#endif

                for(size_t i=0; i<nPackedSprings; ++i) {
                    packedSprings_[i].accumulateJacobians(particles_, x, y);
                }

                for(size_t i=nPackedSprings * 4; i<springs_.size(); ++i) {
                    springs_[i].accumulateJacobians(particles_, x, y);
                }

                for(int i=0; i<n; ++i) {
                    y[i] *= particles_[i].invMass_;
                }

                sxpay(3*n, src, -timeSeconds, dst);
            } else {
                scopy(3*n, src, dst);
            }
        }

        void Cloth::accumulateForces(vec3f* forces) const {
            for(size_t i=0; i<particles_.size(); ++i) {
                ClothParticle const& particle = particles_[i];

                if(0 != particle.invMass_) {
                    forces[i] += vec3f(0,-GRAVITY * particle.mass_,0);
                }
            }

#ifdef USE_SSE
            const size_t nPackedSprings = springs_.size() / 4;
#else
            const size_t nPackedSprings = 0;
#endif
            ClothPackedSpring const*const packedSprings = &packedSprings_[0];

            for(size_t i=0; i<nPackedSprings; ++i) {
                packedSprings[i].accumulateForces(particles_, forces);
            }

            for(size_t i=nPackedSprings * 4; i<springs_.size(); ++i) {
                springs_[i].accumulateForces(particles_, forces);
            }
        }

        void Cloth::simulate(float timeSeconds) {
            Vector forceVector(
                3 * particles_.size(), 
                Vector::Zero);

            vec3f* forces = reinterpret_cast<vec3f*>(forceVector.getRaw());

            accumulateForces(forces);

            Vector deltaVelocityVector(
                3 * particles_.size(), Vector::Zero);

            math::ConjugateGradient cg(64, .001f);

            ImplicitOperatorArg arg(this, timeSeconds);

            for(size_t i=0; i<particles_.size(); ++i) {
                forces[i] *= timeSeconds * particles_[i].invMass_;
            }

            cg.solve(
                3 * particles_.size(), 
                implicitOperator, &arg,
                forceVector, deltaVelocityVector);

            vec3f* deltaVelocity = reinterpret_cast<vec3f*>(deltaVelocityVector.getRaw());

            for(size_t i=0; i<particles_.size(); ++i) {
                particles_[i].velocity_ += deltaVelocity[i];
            }

            if(useStrainLimiting_) {
#ifdef USE_SSE_STRAIN_LIMITING
                size_t nPackedSprings = springs_.size() / 4;
#else
                size_t nPackedSprings = 0;
#endif

                for(size_t i=0; i<nPackedSprings; ++i) {
                    packedSprings_[i].limitStrain(particles_, timeSeconds);
                }

                for(size_t i=nPackedSprings*4; i<springs_.size(); ++i) {
                    springs_[i].limitStrain(particles_, timeSeconds);
                }
            }

            if(collider_) {
                // Modify velocities so that interpenetration won't occur.

                levelset::ILevelSet const& levelSet = collider_->levelSet();

                const float halfThickness = thickness_ / 2.0f;

                for(size_t i=0; i<particles_.size(); ++i) {
                    vec3f p(particles_[i].position_ + particles_[i].velocity_ * timeSeconds);

                    float d = levelSet.value(p);

                    if(d < halfThickness) {
                        vec3f n = levelSet.gradient(p);

                        float dot = vec3f::dot(particles_[i].velocity_, n);

                        if(dot < 0) {
                            vec3f vNormal  = n * dot;
                            vec3f vTangent = particles_[i].velocity_ - vNormal;

                            if(vTangent.length() > 0) {
                                vTangent *= std::max(
                                    0.0f,
                                    1.0f - collider_->frictionCoefficient() 
                                         * (-dot) / vTangent.length());
                            }

                            particles_[i].velocity_ = vTangent;
                        }
                    }
                }
            }

            for(size_t i=0; i<particles_.size(); ++i) {
                ClothParticle& particle = particles_[i];

                particle.position_ += particle.velocity_ * timeSeconds;
            }
        }

        std::vector<ClothParticle> const& Cloth::getParticles() const {
            return particles_;
        }

        std::vector<Cloth::Triangle> const& Cloth::getTriangles() const {
            return triangles_;
        }

        Cloth::Triangle::Triangle(int indices_[3]) {
            for(int i=0; i<3; ++i) {
                indices[i] = indices_[i];
            }
        }

        void Cloth::setParallelizer(boost::shared_ptr<system::IParallelizer> parallelizer) {
            parallelizer_ = parallelizer;
        }

        void Cloth::setCollider(boost::shared_ptr<ClothCollider> collider) {
            collider_ = collider;
        }

        void Cloth::setUseImplicitMethod(bool useImplicitMethod) {
            useImplicitMethod_= useImplicitMethod;
        }

        void Cloth::setUseStrainLimiting(bool useStrainLimiting) {
            useStrainLimiting_ = useStrainLimiting;
        }
    }
}