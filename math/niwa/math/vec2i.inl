/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_VEC2I_INL
#define NIWA_MATH_VEC2I_INL

#include <cmath>

namespace niwa {
    namespace math {
        vec2i::vec2i() : x(0), y(0) {
            // ignored
        }

        vec2i::vec2i(int x_, int y_) 
            : x(x_), y(y_) {
            // ignored
        }

        const vec2i vec2i::operator - () const {
            return vec2i(-x, -y);
        }

        const vec2i vec2i::operator + (vec2i const& rhs) const {
            return vec2i(x + rhs.x, y + rhs.y);
        }

        const vec2i vec2i::operator - (vec2i const& rhs) const {
            return vec2i(x - rhs.x, y - rhs.y);
        }

        const vec2i vec2i::operator * (int rhs) const {
            return vec2i(x * rhs, y * rhs);
        }

        vec2i& vec2i::operator += (vec2i const& rhs) {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        vec2i& vec2i::operator -= (vec2i const& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        vec2i& vec2i::operator *= (int rhs) {
            x *= rhs;
            y *= rhs;
            return *this;
        }
    }
}

#endif