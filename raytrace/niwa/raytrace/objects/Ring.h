/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_OBJECTS_RING_H
#define NIWA_RAYTRACE_OBJECTS_RING_H

#include "niwa/raytrace/AbstractTraceable.h"
#include "niwa/raytrace/Material.h"
#include "niwa/math/vec3f.h"

namespace niwa {
    namespace graphics {
        class Spectrum;
    }

    namespace raytrace {
        class HitInfo;

        namespace objects {
            /**
             * A spinning ring.
             */
            class Ring : public AbstractTraceable {
            public:
                Ring();

                bool __fastcall raytrace(ray3f const& ray, HitInfo& hitInfo) const;

                void computeRotation(float timeSeconds);
            private:
                /**
                 * @return No hit: 0; hit with outer cylinder or caps: 1; hit with inner cylinder (shadow test): 2.
                 */
                int traceRing(
                        ray3f const& ray, graphics::Spectrum& color, 
                        math::vec3f& hitNormal, float& distance) const;

            private:
                math::vec3f spherePosition_;

                float rotation_[3*3];
            };
        }
    }
}

#endif
