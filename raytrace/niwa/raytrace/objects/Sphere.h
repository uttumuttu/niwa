/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_OBJECTS_SPHERE_H
#define NIWA_RAYTRACE_OBJECTS_SPHERE_H

#include "niwa/raytrace/AbstractTraceable.h"
#include "niwa/raytrace/Material.h"
#include "niwa/graphics/Spectrum.h"
#include "niwa/math/vec3f.h"

namespace niwa {
    namespace math {
        class packed_vec3f;
    }

    namespace raytrace {
        class HitInfo;

        namespace objects {
            /**
             * A sphere.
             */
            class Sphere : public AbstractTraceable {
            public:
                Sphere();
                ~Sphere();

                math::vec3f const& getPosition() const;

                float getRadius() const;

                void setPosition(math::vec3f const& position);

                void setRadius(float radius);

                void setMaterial(Material const& material);

                bool __fastcall raytrace(ray3f const& ray, HitInfo& hitInfo) const;

                bool __fastcall raytraceShadow(
                    ray3f const& ray, float cutoffDistance, ILight const* light) const;

                __m128 __fastcall raytraceShadow(
                    packed_ray3f const& ray, __m128 cutoffDistance, ILight const* light) const;

            private:
                /**
                 * Updates the auxiliary radiance
                 * based on other values.
                 */
                void updateRadiance();

            private:
                struct Sse;

            private:
                math::vec3f position_;

                float radius_;
                float squareRadius_; // auxiliary
                float inverseRadius_; // auxiliary

                Material outsideMaterial_;
                Material insideMaterial_;

                Sse* sse_;
            };
        }
    }
}

#endif