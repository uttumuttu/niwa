/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_OBJECTS_SQUARELIGHT_H
#define NIWA_RAYTRACE_OBJECTS_SQUARELIGHT_H

#include "niwa/raytrace/AbstractLight.h"

#include "niwa/graphics/Spectrum.h"

#include "niwa/math/vec3f.h"
#include "niwa/math/packed_vec3f.h"

#include <xmmintrin.h>

namespace niwa {
    namespace raytrace {
        namespace objects {
            class SquareLight : public AbstractLight {
            public:
                SquareLight();
                ~SquareLight();

                void setPosition(math::vec3f const& position);
                void setBasis1(math::vec3f const& basis1);
                void setBasis2(math::vec3f const& basis2);

                void setPower(graphics::Spectrum const& power_);

                math::vec3f const& getPosition() const;
                math::vec3f const& getBasis1() const;
                math::vec3f const& getBasis2() const;

                void setSampleCount(size_t sampleCount);

                size_t getSampleCount() const;

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
                void updateBases();

                void updateRadiance();

                float getArea() const;

                /**
                 * @param u In [0,1).
                 * @param v In [0,1).

                 * @return Position relative to the center of the light.
                 */
                const math::vec3f getRelativePosition(float u, float v) const;

                const math::packed_vec3f getRelativePosition(__m128 u, __m128 v) const;

            private:
                struct Sse;

            private:
                math::vec3f position_;
                math::vec3f basis1_;
                math::vec3f basis2_;

                /**
                 * How many shadow rays are used
                 * for computing direct lighting.
                 */
                size_t sampleCount_;

                float basis1SquareMagnitude_; // auxiliary
                float basis2SquareMagnitude_; // auxiliary

                float area_; // auxiliary

                math::vec3f normal_; // unit surface normal, auxiliary

                graphics::Spectrum power_;

                graphics::Spectrum radiance_; // constant emitted radiance, auxiliary

                Sse* sse_;
            };
        }
    }
}

#endif