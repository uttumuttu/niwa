/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "AbstractTraceable.h"

#include "packed_ray3f.h"

#include "HitInfo.h"

namespace niwa {
    namespace raytrace {
        bool AbstractTraceable::raytraceShadow(
                ray3f const& ray, float cutoffDistance, ILight const* /*light*/) const {
            HitInfo hitInfo = HitInfo::createUninitialized();

            if(raytrace(ray, hitInfo)) {
                return hitInfo.distance() < cutoffDistance;
            } else {
                return false;
            }
        }

        __m128 AbstractTraceable::raytraceShadow(
                packed_ray3f const& ray, __m128 cutoffDistance, ILight const* light) const {
            __m128 result = _mm_setzero_ps();

            for(size_t i=0; i<4; ++i) {
                if(raytraceShadow(ray.get(i), cutoffDistance.m128_f32[i], light)) {
                    result.m128_u32[i] = ~result.m128_u32[i];
                }
            }
            return result;
        }
    }
}
