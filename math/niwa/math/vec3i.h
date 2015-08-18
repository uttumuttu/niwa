/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_VEC3I_H
#define NIWA_MATH_VEC3I_H

namespace niwa {
    namespace math {
        /**
         * A mutable three-dimensional integer vector.
         */
        class vec3i {
        public:
            static vec3i __forceinline createUninitialized();

            /**
             * Creates a vector of zeros.
             */
            inline vec3i();

            inline vec3i(int x_, int y_, int z_);

            inline const vec3i operator - () const;

            inline const vec3i operator + (vec3i const& rhs) const;
            inline const vec3i operator - (vec3i const& rhs) const;
            inline const vec3i operator * (int rhs) const;

            inline vec3i& operator += (vec3i const& rhs);
            inline vec3i& operator -= (vec3i const& rhs);
            inline vec3i& operator *= (int rhs);

            inline const int operator[] (int index) const;

            inline int& operator[] (int index);

        public:
            int x, y, z;
        };
    }
}

#include "vec3i.inl"

#endif
