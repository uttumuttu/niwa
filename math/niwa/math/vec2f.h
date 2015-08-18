/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_VEC2F_H
#define NIWA_MATH_VEC2F_H

namespace niwa {
    namespace math {
        class vec2f {
        public:
            /**
             * Creates a vector of zeros.
             */
            inline vec2f();

            inline vec2f(float x_, float y_);

            inline const vec2f operator - () const;

            inline const vec2f operator + (vec2f const& rhs) const;
            inline const vec2f operator - (vec2f const& rhs) const;
            inline const vec2f operator * (float rhs) const;
            inline const vec2f operator / (float rhs) const;

            inline vec2f& operator += (vec2f const& rhs);
            inline vec2f& operator -= (vec2f const& rhs);
            inline vec2f& operator *= (float rhs);
            inline vec2f& operator /= (float rhs);

            inline void normalize();

            inline float length() const;

            inline float* getRaw();
            inline float const* getRaw() const;

        public:
            static inline float dot(vec2f const& lhs, vec2f const& rhs);

        public:
            float x, y;
        };
    }
}

#include "vec2f.inl"

#endif