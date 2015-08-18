/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_VEC2I_H
#define NIWA_MATH_VEC2I_H

namespace niwa {
    namespace math {
        class vec2i {
        public:
            /**
             * Creates a vector of zeros.
             */
            inline vec2i();

            inline vec2i(int x_, int y_);

            inline const vec2i operator - () const;

            inline const vec2i operator + (vec2i const& rhs) const;
            inline const vec2i operator - (vec2i const& rhs) const;
            inline const vec2i operator * (int rhs) const;

            inline vec2i& operator += (vec2i const& rhs);
            inline vec2i& operator -= (vec2i const& rhs);
            inline vec2i& operator *= (int rhs);

        public:
            int x, y;
        };
    }
}

#include "vec2i.inl"

#endif