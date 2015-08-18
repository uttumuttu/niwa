/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_RAY3F_H
#define NIWA_RAYTRACE_RAY3F_H

#include "niwa/math/vec3f.h"
#include "niwa/math/vec3i.h"

#include <xmmintrin.h>

namespace niwa {
    namespace raytrace {
        /**
         * An immutable three-dimensional ray.
         *
         * Must not be heap-allocated.
         */
        class ray3f {
        public:
            /**
             * @param position Position of the ray.
             *
             * @param direction Must be a unit vector.
             */
            inline ray3f(math::vec3f const& position, math::vec3f const& direction);

            inline const math::vec3f& getPosition() const;

            inline const math::vec3f& getDirection() const;

            /**
             * @return A three-element vector consisting
             *         of the signs of the direction vector.
             *         (Zero for non-negative, one for negative values.)
             */
            inline math::vec3i const& getSigns() const;

            /**
             * @return A three-element vector consisting
             *         of the inverse elements of the direction vector.
             *         (Where division by zero is defined as zero.)
             */
            inline math::vec3f const& getDirectionInverses() const;

        private:
            inline ray3f(
                math::vec3f const& position, math::vec3f const& direction,
                math::vec3i const& signs, math::vec3f const& directionInverses);

        private:
            __m128 position_;
            __m128 direction_; // unit vector

            __m128 signs_; // auxiliary
            __m128 directionInverses_; // auxiliary

            friend class packed_ray3f;
        };
    }
}

#include "ray3f.inl"

#endif