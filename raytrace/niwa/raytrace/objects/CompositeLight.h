/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_COMPOSITELIGHT_H
#define NIWA_RAYTRACE_COMPOSITELIGHT_H

#include "niwa/raytrace/AbstractLight.h"

#include <boost/shared_ptr.hpp>

#include <vector>

namespace niwa {
    namespace raytrace {
        namespace objects {
            class CompositeLight : public AbstractLight {
            public:
                CompositeLight();

                void setLights(
                    std::vector<boost::shared_ptr<ILight>> const& lights);

            public: // from ITraceable
                bool __fastcall raytrace(ray3f const& ray, HitInfo& hitInfo) const;

                bool __fastcall raytraceShadow(
                    ray3f const& ray, float cutoffDistance, ILight const* light) const;

                __m128 __fastcall raytraceShadow(
                    packed_ray3f const& ray, __m128 cutoffDistance, ILight const* light) const;

            public: // from ILight
                const graphics::Spectrum __fastcall getPower() const;

                const graphics::Spectrum __fastcall sampleIrradiance(
                    math::vec3f const& position,
                    math::vec3f const& normal,
                    ITraceable const& scene) const;

                const ray3f __fastcall samplePhoton(graphics::Spectrum& power,
                    math::vec2f const& positionParameter,
                    math::vec2f const& directionParameter) const;

            private:
                std::vector<boost::shared_ptr<ILight>> lights_;
            };
        }
    }
}

#endif