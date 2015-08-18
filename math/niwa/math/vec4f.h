/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_VEC4F_H
#define NIWA_MATH_VEC4F_H

namespace niwa {
    namespace math {
        /**
         * A mutable four-dimensional single-precision vector.
         */
        class vec4f {
        public:
            /**
             * Creates a vector of zeros.
             */
            inline vec4f();

            inline vec4f(float x_, float y_, float z_, float w_);

            inline const vec4f operator - () const;

            inline const vec4f operator + (vec4f const& rhs) const;
            inline const vec4f operator - (vec4f const& rhs) const;
            inline const vec4f operator * (float rhs) const;
            inline const vec4f operator / (float rhs) const;

            inline vec4f& operator += (vec4f const& rhs);
            inline vec4f& operator -= (vec4f const& rhs);
            inline vec4f& operator *= (float rhs);
            inline vec4f& operator /= (float rhs);

            inline void normalize();

            inline float length() const;

        public:
            static inline float dot(vec4f const& lhs, vec4f const& rhs);

        public:
            float x, y, z, w;
        };
    }
}

#include "vec4f.inl"

#endif
