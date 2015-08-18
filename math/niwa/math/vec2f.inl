/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_VEC2F_INL
#define NIWA_MATH_VEC2F_INL

#include <cmath>

namespace niwa {
    namespace math {
        vec2f::vec2f() : x(0), y(0) {
            // ignored
        }

        vec2f::vec2f(float x_, float y_) 
            : x(x_), y(y_) {
            // ignored
        }

        const vec2f vec2f::operator - () const {
            return vec2f(-x, -y);
        }

        const vec2f vec2f::operator + (vec2f const& rhs) const {
            return vec2f(x + rhs.x, y + rhs.y);
        }

        const vec2f vec2f::operator - (vec2f const& rhs) const {
            return vec2f(x - rhs.x, y - rhs.y);
        }

        const vec2f vec2f::operator * (float rhs) const {
            return vec2f(x * rhs, y * rhs);
        }

        const vec2f vec2f::operator / (float rhs) const {
            return *this * ((rhs == 0) ? 1 : 1/rhs);
        }

        vec2f& vec2f::operator += (vec2f const& rhs) {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        vec2f& vec2f::operator -= (vec2f const& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        vec2f& vec2f::operator *= (float rhs) {
            x *= rhs;
            y *= rhs;
            return *this;
        }

        vec2f& vec2f::operator /= (float rhs) {
            return *this *= ((rhs == 0) ? 1 : 1/rhs);
        }

        void vec2f::normalize() {
            *this /= length();
        }

        float vec2f::length() const {
            return sqrt(x*x + y*y);
        }

        float vec2f::dot(vec2f const& lhs, vec2f const& rhs) {
            return lhs.x * rhs.x + lhs.y * rhs.y;
        }

        float* vec2f::getRaw() {
            return &x;
        }

        float const* vec2f::getRaw() const {
            return &x;
        }
    }
}

#endif