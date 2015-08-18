/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/PhotonTracer.h"

#include "niwa/raytrace/HitInfo.h"
#include "niwa/raytrace/ILight.h"
#include "niwa/raytrace/ITraceable.h"
#include "niwa/raytrace/Hemisphere.h"
#include "niwa/raytrace/ray3f.h"

#include "niwa/photonmap/IPhotonMap.h"
#include "niwa/photonmap/Photon.h"

#include "niwa/graphics/Spectrum.h"

#include "niwa/math/vec2f.h"

#include "niwa/random/Lcg.h"
#include "niwa/random/RandomSet.h"
#include "niwa/random/Halton.h"

#include "niwa/system/IParallelizer.h"

#define RANDOM_DIMENSION 4

namespace {
    float randomAt(float* randomVector, int index) {
        return index < RANDOM_DIMENSION 
            ? randomVector[index] 
            : niwa::random::Lcg::global().nextf();
    }
}

using boost::shared_ptr;

namespace niwa {
    using graphics::Spectrum;

    using math::vec2f;
    using math::vec3f;

    using photonmap::Photon;
    using photonmap::IPhotonMap;

    using random::Lcg;

    namespace raytrace {
        class PhotonTracer::TraceTask : public system::IParallelizer::ICallback {
        public:
            TraceTask(
                PhotonTracer const& parent, 
                IPhotonMap& photonMap, int photonCount);

            void __fastcall invoke(int i);

        private: // prevent copying
            TraceTask(TraceTask const&);
            TraceTask& operator = (TraceTask const&);

        private:
            PhotonTracer const& parent_;
            IPhotonMap& photonMap_;
            const int photonCount_;
        };

        PhotonTracer::PhotonTracer() {
            // ignored
        }

        PhotonTracer::~PhotonTracer() {
            // ignored
        }

        void PhotonTracer::setScene(shared_ptr<ITraceable> scene) {
            scene_ = scene;
        }

        void PhotonTracer::setLight(shared_ptr<ILight> light) {
            light_ = light;
        }

        void PhotonTracer::tracePhotons(IPhotonMap& photonMap, int photonCount) const {
            if(!scene_ || !light_ || !parallelizer_) {
                return;
            }

            TraceTask task(*this, photonMap, photonCount);

            randomSet_ = std::auto_ptr<random::RandomSet>(
                random::Halton::createHaltonHammersleySet(
                    RANDOM_DIMENSION, photonCount, true));

            parallelizer_->loop(task, 0, photonCount);
        }

        PhotonTracer::TraceTask::TraceTask(
                PhotonTracer const& parent,
                IPhotonMap& photonMap,
                int photonCount)
            : parent_(parent), photonMap_(photonMap), 
              photonCount_(photonCount) {
            // ignored
        }

        void PhotonTracer::setParallelizer(shared_ptr<system::IParallelizer> parallelizer) {
            parallelizer_ = parallelizer;
        }

        void PhotonTracer::TraceTask::invoke(int /*index*/) {
            parent_.traceSinglePhoton(photonMap_, photonCount_);
        }

        void PhotonTracer::traceSinglePhoton(IPhotonMap& photonMap, int photonCount) const {
            float randomVector[RANDOM_DIMENSION];

            randomSet_->nextf(randomVector);

            Spectrum power;

            ray3f ray(light_->samplePhoton(
                power,
                vec2f(
                    Lcg::global().nextf(),
                    Lcg::global().nextf()),
                vec2f(
                    randomAt(randomVector, 0), 
                    randomAt(randomVector, 1))));

            power /= static_cast<float>(photonCount); // split power

            HitInfo hit = HitInfo::createUninitialized();

            float currentRefractiveIndex = 1;

            int bounceCount = 0;

            while( scene_->raytrace(ray, hit) ) {
                if(bounceCount > 0) {
                    Photon photon(
                        hit.position(),
                        hit.normal(),
                        power);

                    photonMap.add(photon);
                }

                Material const& material = hit.material();

                if(material.getType() == Material::MATERIAL_DIFFUSE) {
                    float u = Lcg::global().nextf();

                    // Russian roulette.
                    if(u <= material.getReflectance().average()) {
                        power *= material.getReflectance() 
                            / material.getReflectance().average();

                        ray = ray3f(
                            hit.position(),
                            Hemisphere::cosineWeightedDirection(
                                hit.normal(),
                                randomAt(randomVector, 2+2*bounceCount),
                                randomAt(randomVector, 3+2*bounceCount)));
                    } else {
                        break;
                    }
                } else if(material.getType() == Material::MATERIAL_SPECULAR) {
                    float u = Lcg::global().nextf();

                    // Russian roulette.
                    if(u <= material.getReflectance().average()) {
                        power *= material.getReflectance() 
                            / material.getReflectance().average();

                        ray = ray3f(
                            hit.position(),
                            Hemisphere::mirrorReflection(
                                hit.normal(),
                                ray.getDirection()));
                    } else {
                        break;
                    }
                } else if(material.getType() == Material::MATERIAL_DIELECTRIC) {
                    float fresnel = Hemisphere::fresnelCoefficient(
                        hit.normal(), ray.getDirection(),
                        currentRefractiveIndex,
                        material.getRefractiveIndex());

                    float u = Lcg::global().nextf();

                    if(u < 1 - fresnel) {
                        // refraction

                        vec3f excitantDirection;

                        if( Hemisphere::computeRefraction(
                                hit.normal(), ray.getDirection(),
                                currentRefractiveIndex,
                                material.getRefractiveIndex(),
                                excitantDirection) ) {
                            ray = ray3f(
                                hit.position(),
                                excitantDirection);

                            currentRefractiveIndex = material.getRefractiveIndex();
                        } else {
                            // total internal reflection

                            ray = ray3f(
                                hit.position(),
                                Hemisphere::mirrorReflection(
                                    hit.normal(),
                                    ray.getDirection()));
                        }
                    } else {
                        // reflection

                        ray = ray3f(
                            hit.position(),
                            Hemisphere::mirrorReflection(
                                hit.normal(),
                                ray.getDirection()));
                    }
                } else {
                    break;
                }

                ++bounceCount;
            }
        }
    }
}