/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_VEC3I_INL
#define NIWA_MATH_VEC3I_INL

namespace niwa {
    namespace math {
#pragma warning(push)
#pragma warning(disable:4700)
        vec3i vec3i::createUninitialized() {
            unsigned char space[sizeof(vec3i)];
            return *reinterpret_cast<vec3i*>(space);
        }
#pragma warning(pop)

        vec3i::vec3i() : x(0), y(0), z(0) {
            // ignored
        }

        vec3i::vec3i(int x_, int y_, int z_) 
            : x(x_), y(y_), z(z_) {
            // ignored
        }

        const vec3i vec3i::operator - () const {
            return vec3i(-x, -y, -z);
        }

        const vec3i vec3i::operator + (vec3i const& rhs) const {
            return vec3i(x + rhs.x, y + rhs.y, z + rhs.z);
        }

        const vec3i vec3i::operator - (vec3i const& rhs) const {
            return vec3i(x - rhs.x, y - rhs.y, z - rhs.z);
        }

        const vec3i vec3i::operator * (int rhs) const {
            return vec3i(x * rhs, y * rhs, z * rhs);
        }

        vec3i& vec3i::operator += (vec3i const& rhs) {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        vec3i& vec3i::operator -= (vec3i const& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }

        vec3i& vec3i::operator *= (int rhs) {
            x *= rhs;
            y *= rhs;
            z *= rhs;
            return *this;
        }

        const int vec3i::operator[] (int index) const {
            return (&x)[index];
        }

        int& vec3i::operator[] (int index) {
            return (&x)[index];
        }
    }
}

#endif