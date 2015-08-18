/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_PACKED_RAY3F_H
#define NIWA_RAYTRACE_PACKED_RAY3F_H

#include "niwa/math/packed_vec3f.h"
#include "niwa/math/packed_vec3i.h"

#include "ray3f.h"

namespace niwa {
    namespace raytrace {
        /**
         * A collection of four three-dimensional rays.
         *
         * Must not be heap-allocated.
         */
        class packed_ray3f {
        public:
            /**
             * Creates four rays with zero fields.
             */
            inline packed_ray3f();

            inline packed_ray3f(
                math::packed_vec3f const& position, 
                math::packed_vec3f const& direction);

            inline void set(int index, ray3f const& ray);

            inline ray3f get(int index) const;

            inline math::packed_vec3f const& getPosition() const;

            inline math::packed_vec3f const& getDirection() const;

            /**
             * @return A three-element vector consisting
             *         of the signs of the direction vector.
             *         (Zero for non-negative, one for negative values.)
             */
            inline math::packed_vec3i const& getSigns() const;

            /**
             * @return A three-element vector consisting
             *         of the inverse elements of the direction vector.
             *         (Where division by zero is defined as zero.)
             */
            inline math::packed_vec3f const& getDirectionInverses() const;
        private:
            math::packed_vec3f position_;

            math::packed_vec3f direction_;

            math::packed_vec3i signs_; // auxiliary

            math::packed_vec3f directionInverses_; // auxiliary
        };
    }
}

#include "packed_ray3f.inl"

#endif