/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_ABSTRACTTRACEABLE_H
#define NIWA_RAYTRACE_ABSTRACTTRACEABLE_H

#include "ITraceable.h"

namespace niwa {
    namespace raytrace {
        /**
         * A traceable with default (non-optimized)
         * implementations for all functions except
         * the core tracing operation.
         */
        class AbstractTraceable : public ITraceable {
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