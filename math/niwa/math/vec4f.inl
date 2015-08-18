/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_VEC4F_INL
#define NIWA_MATH_VEC4F_INL

namespace niwa {
    namespace math {
        vec4f::vec4f() : x(0), y(0), z(0), w(0) {
            // ignored
        }

        vec4f::vec4f(float x_, float y_, float z_, float w_) 
            : x(x_), y(y_), z(z_), w(w_) {
            // ignored
        }

        const vec4f vec4f::operator - () const {
            return vec4f(-x, -y, -z, -w);
        }

        const vec4f vec4f::operator + (vec4f const& rhs) const {
            return vec4f(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
        }

        const vec4f vec4f::operator - (vec4f const& rhs) const {
            return vec4f(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
        }

        const vec4f vec4f::operator * (float rhs) const {
            return vec4f(x * rhs, y * rhs, z * rhs, w * rhs);
        }

        const vec4f vec4f::operator / (float rhs) const {
            return *this * ((rhs == 0) ? 1 : 1/rhs);
        }

        vec4f& vec4f::operator += (vec4f const& rhs) {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            w += rhs.w;
            return *this;
        }

        vec4f& vec4f::operator -= (vec4f const& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            w -= rhs.w;
            return *this;
        }

        vec4f& vec4f::operator *= (float rhs) {
            x *= rhs;
            y *= rhs;
            z *= rhs;
            w *= rhs;
            return *this;
        }

        vec4f& vec4f::operator /= (float rhs) {
            return *this *= ((rhs == 0) ? 1 : 1/rhs);
        }

        void vec4f::normalize() {
            *this /= length();
        }

        float vec4f::length() const {
            return sqrt(x*x + y*y + z*z + w*w);
        }

        float vec4f::dot(vec4f const& lhs, vec4f const& rhs) {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
        }
    }
}

#endif