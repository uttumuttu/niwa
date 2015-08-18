/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "CompositeLight.h"

#include "niwa/raytrace/HitInfo.h"

#include "niwa/random/Lcg.h"

namespace niwa {
    using graphics::Spectrum;

    using math::vec2f;

    namespace raytrace {
        namespace objects {
            CompositeLight::CompositeLight() {
                // ignored
            }

            void CompositeLight::setLights(
                std::vector<boost::shared_ptr<ILight>> const& lights) {
                lights_ = lights;
            }

            const Spectrum CompositeLight::sampleIrradiance(
                    math::vec3f const& position,
                    math::vec3f const& normal,
                    ITraceable const& scene) const {
                Spectrum irradiance(0,0,0);

                const size_t n = lights_.size();

                boost::shared_ptr<ILight> const*const lights = &lights_[0];

                for(size_t i=0; i<n; ++i) {
                    irradiance += lights[i]->sampleIrradiance(
                        position, normal, scene);
                }

                return irradiance;
            }

            const Spectrum CompositeLight::getPower() const {
                Spectrum power(0,0,0);

                const size_t n = lights_.size();

                boost::shared_ptr<ILight> const*const lights = &lights_[0];

                for(size_t i=0; i<n; ++i) {
                    power += lights[i]->getPower();
                }

                return power;
            }

            const ray3f CompositeLight::samplePhoton(
                    Spectrum& power,
                    vec2f const& positionParameter,
                    vec2f const& directionParameter) const {
                const size_t nLights = lights_.size();

                boost::shared_ptr<ILight> const*const lights = &lights_[0];

                if(nLights == 0) {
                    float d = sqrt(1.0f / 3);

                    return ray3f(math::vec3f(0,0,0), math::vec3f(d,d,d));
                } else if(nLights == 1) {
                    return lights[0]->samplePhoton(
                        power, positionParameter, directionParameter);
                }

                float totalPower = 0;

                for(size_t i=0; i<nLights; ++i) {
                    totalPower += lights[i]->getPower().average();
                }

                float cutoff = totalPower * niwa::random::Lcg::global().nextf();

                float running = 0;

                for(size_t i=0; i<nLights; ++i) {
                    float localPower = lights[i]->getPower().average();

                    running += localPower;

                    if(running >= cutoff) {
                        ray3f ray = lights[i]->samplePhoton(
                            power, positionParameter, directionParameter);

                        // The photon was importance sampled,
                        // so divide by PDF. (For efficiency, 
                        // we actually multiply by inverse PDF.)
                        power *= (totalPower / localPower);

                        return ray;
                    }
                }

                float localPower = lights[nLights-1]->getPower().average();

                ray3f ray = lights[nLights-1]->samplePhoton(
                    power, positionParameter, directionParameter);

                // The photon was importance sampled,
                // so divide by PDF (rather, multiply by inverse PDF).
                power *= (totalPower / localPower);

                return ray;
            }

            bool CompositeLight::raytrace(ray3f const& ray, HitInfo& hitInfo) const {
                HitInfo candidateHit = HitInfo::createUninitialized();

                bool hitFound = false;

                const size_t n = lights_.size();

                boost::shared_ptr<ILight> const*const lights = &lights_[0];

                for(size_t i=0; i<n; ++i) {
                    if( lights[i]->raytrace(ray, candidateHit) ) {
                        if(!hitFound || candidateHit.distance() < hitInfo.distance()) {
                            hitInfo = candidateHit;

                            hitFound = true;
                        }
                    }
                }

                return hitFound;
            }

            bool CompositeLight::raytraceShadow(
                ray3f const& ray, float cutoffDistance, ILight const* light) const {
                const size_t n = lights_.size();

                boost::shared_ptr<ILight> const*const lights = &lights_[0];

                for(size_t i=0; i<n; ++i) {
                    if( lights[i]->raytraceShadow(ray, cutoffDistance, light) ) {
                        return true;
                    }
                }
                return false;
            }

            __m128 CompositeLight::raytraceShadow(
                packed_ray3f const& ray, 
                const __m128 cutoffDistance, 
                ILight const* const light) const {
                __m128 mask = _mm_setzero_ps();

                const size_t n = lights_.size();

                boost::shared_ptr<ILight> const*const lights = &lights_[0];

                for(size_t i=0; i<n; ++i) {
                    mask = _mm_or_ps(mask,
                        lights[i]->raytraceShadow(ray, cutoffDistance, light));

                    // If all rays are found to be shadowed, return early.
                    if(mask.m128_i32[0] && mask.m128_i32[1]
                        && mask.m128_i32[2] && mask.m128_i32[3]) {
                        return mask;
                    }
                }
                return mask;
            }
        }
    }
}