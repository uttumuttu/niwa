/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_OBJECTS_CORNELLBOXWALLS_H
#define NIWA_RAYTRACE_OBJECTS_CORNELLBOXWALLS_H

#include "niwa/raytrace/AbstractTraceable.h"

#include "niwa/raytrace/Material.h"

namespace niwa {
    namespace raytrace {
        namespace objects {
            /**
             * Highly optimized walls for a Cornell box.
             */
            class CornellBoxWalls : public AbstractTraceable {
            public:
                CornellBoxWalls();

                bool __fastcall raytrace(ray3f const& ray, HitInfo& hitInfo) const;

                bool __fastcall raytraceShadow(
                    ray3f const& ray, float cutoffDistance, ILight const* light) const;

                __m128 __fastcall raytraceShadow(
                    packed_ray3f const& ray, __m128 cutoffDistance, ILight const* light) const;

            private:
                Material defaultMaterial_;
                Material leftWall_;
                Material rightWall_;
            };
        }
    }
}

#endif