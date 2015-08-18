/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_ITRACEABLE_H
#define NIWA_RAYTRACE_ITRACEABLE_H

#include <xmmintrin.h>

namespace niwa {
    namespace raytrace {
        class ray3f;
        class packed_ray3f;

        class HitInfo;
        class ILight;

        class ITraceable {
        public:
            ITraceable();

            virtual ~ITraceable();

            /**
             * Full raytracing operation.
             *
             * @param ray The ray to be traced.
             *
             * @param hitInfo If the function returns true, will contain
             *                information about the hit.
             *
             * @return Whether the ray hit the traceable object.
             */
            virtual bool __fastcall raytrace(ray3f const& ray, HitInfo& hitInfo) const = 0;

            /**
             * Shadow-ray casting.
             *
             * @param ray The ray to be traced.
             *
             * @param cutoffDistance The distance that is considered for shadow checking.
             *                       Controlled for rounding errors.
             *
             * @param light The light this shadow-ray associates with, or NULL
             *              if the shadow ray doesn't associate with any light.
             *              (This parameter should be used for optimizations only.)
             *
             * @return Whether the ray hits the traceable at a distance < cutoffDistance.
             */
            virtual bool __fastcall raytraceShadow(
                ray3f const& ray, 
                float cutoffDistance,
                ILight const* light) const = 0;

            /**
             * Packed shadow-ray casting.
             *
             * @param ray The rays to be traced.
             *
             * @param cutoffDistances The distances that are considered for shadow checking.
             *                        Controlled for rounding errors.
             *
             * @param light The light the shadow-rays associate with, or NULL
             *              if the shadow rays don't associate with any light.
             *              (This parameter should be used for optimizatiosn only.)
             *
             * @return Four packed booleans indicating whether each ray hits
             *         the traceable at a distance < cutoffDistance.
             */
            virtual __m128 __fastcall raytraceShadow(
                packed_ray3f const& ray,
                __m128 cutoffDistance,
                ILight const* light) const = 0;

        private: // prevent slicing and copying
            ITraceable(ITraceable const&);
            ITraceable& operator = (ITraceable const&);
        };
    }
}

#endif
