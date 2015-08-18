/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_ABSTRACTLIGHT_H
#define NIWA_RAYTRACE_ABSTRACTLIGHT_H

#include "ILight.h"

namespace niwa {
    namespace raytrace {
        /**
         * A light with default (non-optimized)
         * implementations for all ray tracing functions except
         * the core tracing operation.
         */
        class AbstractLight : public ILight {
        public:
            /**
             * Implemented in terms of raytrace(...) function.
             */
            bool __fastcall raytraceShadow(
                ray3f const& ray, float cutoffDistance, ILight const* light) const;

            /**
             * Implemented in terms of raytraceShadow(...) function.
             */
            __m128 __fastcall raytraceShadow(
                packed_ray3f const& ray,
                __m128 cutoffDistance,
                ILight const* light) const;
        };
    }
}

#endif