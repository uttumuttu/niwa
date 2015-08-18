/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "ClothSpring.h"

#include "niwa/math/packed_vec3f.h"

//#define USE_PROJECTIVE_DAMPING

#define MAX_SPRING_COMPRESSION 0.0f

#define MAX_SPRING_EXTENSION 0.1f

using niwa::math::packed_vec3f;

/**
 * This compilation unit uses only packed SSE operations,
 * so we'll use operator overloading as a shorthand.
 */
namespace {
    static __forceinline __m128 __fastcall operator * (
        __m128 const& lhs, __m128 const& rhs) {
        return _mm_mul_ps(lhs, rhs);
    }

    static __forceinline __m128 __fastcall operator - (
        __m128 const& lhs, __m128 const& rhs) {
            return _mm_sub_ps(lhs, rhs);
    }

    static __forceinline __m128 __fastcall operator / (
        __m128 const& lhs, __m128 const& rhs) {
            return _mm_div_ps(lhs, rhs);
    }
}

namespace niwa {
    namespace dynamics {
        using math::vec3f;

        ClothSpring::ClothSpring(
                std::vector<ClothParticle> const& particles,
                int firstIndex, int secondIndex,
                float integralStiffness,
                float integralDampingCoefficient) {
            indices_[0] = firstIndex;
            indices_[1] = secondIndex;

            vec3f const& p0 = particles[indices_[0]].getPosition();
            vec3f const& p1 = particles[indices_[1]].getPosition();

            restLength_ = vec3f(p0 - p1).length();

            stiffness_ = integralStiffness / restLength_;
            dampingCoefficient_ = integralDampingCoefficient / restLength_;
        }

        void ClothSpring::accumulateForces(
                std::vector<ClothParticle> const& particles,
                vec3f* forces) const {
            ClothParticle const& p0 = particles[indices_[0]];
            ClothParticle const& p1 = particles[indices_[1]];

            const vec3f relativePosition(p0.position_ - p1.position_);

            const vec3f relativeVelocity(p0.velocity_ - p1.velocity_);

            float invLength = 1.0f / sqrt(vec3f::dot(relativePosition, relativePosition));

#ifdef USE_PROJECTIVE_DAMPING
            vec3f force(
                relativePosition * (stiffness_ * (restLength_ * invLength - 1.0f)
                - dampingCoefficient_ * vec3f::dot(relativeVelocity, relativePosition) * 
                (invLength * invLength)));
#else
            vec3f force(
                relativePosition * (stiffness_ * (restLength_ * invLength - 1.0f))
                - relativeVelocity * dampingCoefficient_);
#endif

            forces[indices_[0]] += force;
            forces[indices_[1]] -= force;
        }

        void ClothPackedSpring::accumulateForces(
                std::vector<ClothParticle> const& particles,
                vec3f* forces) const {
            packed_vec3f pos[2];
            packed_vec3f vel[2];

            for(int i=0; i<4; ++i) {
                ClothParticle const& p0 = particles[indices_[i*2]];
                ClothParticle const& p1 = particles[indices_[i*2+1]];

                pos[0].set(i, p0.position_);
                pos[1].set(i, p1.position_);

                vel[0].set(i, p0.velocity_);
                vel[1].set(i, p1.velocity_);
            }

            packed_vec3f relativePosition = pos[0] - pos[1];
            packed_vec3f relativeVelocity = vel[0] - vel[1];

            __m128 invLength = _mm_rsqrt_ps(relativePosition.squareLength());

            __declspec(align(16)) static const float ones[4] = {1.0f, 1.0f, 1.0f, 1.0f};

            __m128 const* one = reinterpret_cast<__m128 const*>(ones);

#ifdef USE_PROJECTIVE_DAMPING
            packed_vec3f force(
                relativePosition * (stiffness_ * (restLength_ * invLength - *one)
                - dampingCoefficient_ * relativeVelocity.dot(relativePosition) * 
                (invLength * invLength)));
#else
            packed_vec3f force(
                relativePosition * (stiffness_ * (restLength_ * invLength - *one))
                - relativeVelocity * dampingCoefficient_);
#endif

            for(int j=0; j<4; ++j) {
                const vec3f f = force.get(j);

                forces[indices_[j*2  ]] += f;
                forces[indices_[j*2+1]] -= f;
            }
        }

        void ClothSpring::accumulateJacobians(
                std::vector<ClothParticle> const& 
#ifdef USE_PROJECTIVE_DAMPING
                particles, 
#else
                /*particles*/,
#endif
                math::vec3f const* src, math::vec3f* dst) const {
#ifdef USE_PROJECTIVE_DAMPING
            vec3f jacobian(
                particles[indices_[0]].position_ 
                - particles[indices_[1]].position_);

            jacobian *= 
                vec3f::dot(jacobian, src[indices_[0]] - src[indices_[1]])
                * dampingCoefficient_ / jacobian.squareLength();
#else
            vec3f jacobian(src[indices_[0]] - src[indices_[1]]);

            jacobian *= dampingCoefficient_;
#endif
            dst[indices_[0]] -= jacobian;
            dst[indices_[1]] += jacobian;
        }

        void ClothPackedSpring::accumulateJacobians(
            std::vector<ClothParticle> const& 
#ifdef USE_PROJECTIVE_DAMPING
            particles,
#else
            /*particles*/,
#endif
            vec3f const* src, vec3f* dst) const {
#ifdef USE_PROJECTIVE_DAMPING
            packed_vec3f pos[2];

            for(int i=0; i<4; ++i) {
                ClothParticle const& p0 = particles[indices_[i*2]];
                ClothParticle const& p1 = particles[indices_[i*2+1]];

                pos[0].set(i, p0.position_);
                pos[1].set(i, p1.position_);
            }

            packed_vec3f relativePosition = pos[0] - pos[1];

            packed_vec3f x[2];

            for(int j=0; j<4; ++j) {
                x[0].set(j, src[indices_[j*2]]);
                x[1].set(j, src[indices_[j*2+1]]);
            }

            packed_vec3f jacobian(relativePosition);

            jacobian *=
                jacobian.dot(x[0] - x[1])
                * dampingCoefficient_ / jacobian.squareLength();
#else
            packed_vec3f x[2];

            for(int j=0; j<4; ++j) {
                x[0].set(j, src[indices_[j*2]]);
                x[1].set(j, src[indices_[j*2+1]]);
            }

            packed_vec3f jacobian(x[0] - x[1]);

            jacobian *= dampingCoefficient_;
#endif
            for(int j=0; j<4; ++j) {
                const vec3f component = jacobian.get(j);

                dst[indices_[j*2]] -= component;
                dst[indices_[j*2+1]] += component;
            }
        }

        void ClothPackedSpring::setup(std::vector<ClothSpring>::const_iterator it) {
            for(int i=0; i<4; ++i, ++it) {
                indices_[i*2  ] = it->indices_[0];
                indices_[i*2+1] = it->indices_[1];

                restLength_.m128_f32[i] = it->restLength_;
                stiffness_.m128_f32[i] = it->stiffness_;
                dampingCoefficient_.m128_f32[i] = it->dampingCoefficient_;
            }
        }

        /**
         * We use strain limit algorithm from
         * Bridson et al's 2003 paper
         * "Simulation of Cloth with Folds and Wrinkles".
         *
         * This algorithm states that spring's shouldn't
         * stretch much beyond of their rest length
         * nor compress much below their rest length
         * (the latter condition promotes the formation
         * of visually pleasing folds and wrinkles).
         *
         * Notice the procedure is sequential;
         * that is, the order of springs' strain
         * limitations may have an effect.
         * (Unfortunately, parallel relaxation
         * isn't as robust.)
         */
        void ClothSpring::limitStrain(
                std::vector<ClothParticle>& particles,
                float timeSeconds) const {
            ClothParticle& p0 = particles[indices_[0]];
            ClothParticle& p1 = particles[indices_[1]];

            vec3f relativePosition(p0.position_ - p1.position_);
            vec3f relativeVelocity(p0.velocity_ - p1.velocity_);

            float length = relativePosition.length();

            // We use the word "speed" to denote
            // one-dimensional (projected) velocity.
            float tangentialSpeed = vec3f::dot(
                relativePosition, relativeVelocity) / length;

            // The maximum time step that can be
            // taken without causing excess tangential
            // strain.
            float maxTime = timeSeconds;

            if(tangentialSpeed > 0) {
                const float maxLength =
                    restLength_ * (1 + MAX_SPRING_EXTENSION);

                maxTime = std::min(maxTime,
                    std::max(maxLength - length, 0.0f) / tangentialSpeed);
            } else if(tangentialSpeed < 0) {
                const float minLength =
                    restLength_ * (1 - MAX_SPRING_COMPRESSION);

                maxTime = std::min(maxTime,
                    std::min(minLength - length, 0.0f) / tangentialSpeed);
            }

            if(maxTime < timeSeconds) {
                vec3f tangentialVelocity(
                    relativePosition * (tangentialSpeed / length));

                vec3f impulse(
                    tangentialVelocity * (1 - maxTime / timeSeconds));

                if(p0.invMass_ > 0 && p1.invMass_ > 0) {
                    impulse *= .5f;

                    p0.velocity_ -= impulse;
                    p1.velocity_ += impulse;
                }/* else if(p0.invMass_ > 0) {
                    p0.velocity_ -= impulse;
                } else if(p1.invMass_ > 0) {
                    p1.velocity_ += impulse;
                }*/
            }
        }

        void ClothPackedSpring::limitStrain(
                std::vector<ClothParticle>& particles,
                float timeSeconds) const {
            packed_vec3f pos[2];
            packed_vec3f vel[2];

            for(int i=0; i<4; ++i) {
                ClothParticle& p0 = particles[indices_[i*2]];
                ClothParticle& p1 = particles[indices_[i*2+1]];

                pos[0].set(i, p0.position_);
                pos[1].set(i, p1.position_);

                vel[0].set(i, p0.velocity_);
                vel[1].set(i, p1.velocity_);
            }

            packed_vec3f relativePositions(pos[0] - pos[1]);
            packed_vec3f relativeVelocitys(vel[0] - vel[1]);

            __m128 lengths = relativePositions.length();

            __m128 invLengths = _mm_rcp_ps(lengths);

            // We use the word "speed" to denote
            // one-dimensional (projected) velocity.
            __m128 tangentialSpeeds = relativePositions.dot(
                relativeVelocitys) * invLengths;

            __m128 invTangentialSpeeds = _mm_rcp_ps(tangentialSpeeds);

            __m128 maxLengths = 
                restLength_ * _mm_set_ps1(1 + MAX_SPRING_EXTENSION);

            __m128 minLengths = 
                restLength_ * _mm_set_ps1(1 - MAX_SPRING_EXTENSION);

            __m128 extensionTimes =
                _mm_max_ps(maxLengths - lengths, _mm_setzero_ps())
                * invTangentialSpeeds;

            __m128 compressionTimes =
                _mm_min_ps(minLengths - lengths, _mm_setzero_ps())
                * invTangentialSpeeds;

            for(int i=0; i<4; ++i) {
                // The maximum time step that can be
                // taken without causing excess tangential
                // strain.
                float maxTime = timeSeconds;

                float tangentialSpeed = tangentialSpeeds.m128_f32[i];

                if(tangentialSpeed > 0) {
                    maxTime = extensionTimes.m128_f32[i];
                } else if(tangentialSpeed < 0) {
                    maxTime = compressionTimes.m128_f32[i];
                }

                if(maxTime < timeSeconds) {
                    ClothParticle& p0 = particles[indices_[i*2]];
                    ClothParticle& p1 = particles[indices_[i*2+1]];

                    float invLength = invLengths.m128_f32[i];

                    vec3f relativePosition = relativePositions.get(i);

                    vec3f tangentialVelocity(
                        relativePosition * (tangentialSpeed * invLength));

                    vec3f impulse(
                        tangentialVelocity * (1 - maxTime / timeSeconds));

                    if(p0.invMass_ > 0 && p1.invMass_ > 0) {
                        impulse *= .5f;

                        p0.velocity_ -= impulse;
                        p1.velocity_ += impulse;
                    } else if(p0.invMass_ > 0) {
                        p0.velocity_ -= impulse;
                    } else if(p1.invMass_ > 0) {
                        p1.velocity_ += impulse;
                    }
                }
            }
        }

        ClothPackedSpring::ClothPackedSpring() {
            // ignored
        }

        void* ClothPackedSpring::operator new[] (size_t sz) {
            void* result = _mm_malloc(sz, 16);

            if(!result) {
                throw std::bad_alloc();
            } else {
                return result;
            }
        }

        void ClothPackedSpring::operator delete[] (void* p) {
            _mm_free(p);
        }

        bool ClothSpring::operator < (ClothSpring const& rhs) const {
            if(indices_[0] < rhs.indices_[0]) {
                return true;
            } else if(indices_[0] == rhs.indices_[0]) {
                return indices_[1] < rhs.indices_[1];
            } else {
                return false;
            }
        }
    }
}