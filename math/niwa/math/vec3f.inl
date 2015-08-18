/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_VEC3F_INL
#define NIWA_MATH_VEC3F_INL

#include <cmath>

namespace niwa {
    namespace math {
#pragma warning(push)
#pragma warning(disable:4700)
        vec3f vec3f::createUninitialized() {
            // TODO: the uninitialized code fails with debugging; find a workaround!
            return vec3f();
            //unsigned char space[sizeof(vec3f)];
            //return *reinterpret_cast<vec3f*>(space);
        }
#pragma warning(pop)

        vec3f::vec3f() : x(0), y(0), z(0) {
            // ignored
        }

        vec3f::vec3f(float x_, float y_, float z_) 
            : x(x_), y(y_), z(z_) {
            // ignored
        }

        vec3f::vec3f(float* values) 
            : x(values[0]), y(values[1]), z(values[2]) {
            // ignored
        }

#ifndef NIWA_MATH_USE_TEMPLATES
        const vec3f vec3f::operator - () const {
            return vec3f(-x, -y, -z);
        }

        const vec3f vec3f::operator + (vec3f const& rhs) const {
            return vec3f(x + rhs.x, y + rhs.y, z + rhs.z);
        }

        const vec3f vec3f::operator - (vec3f const& rhs) const {
            return vec3f(x - rhs.x, y - rhs.y, z - rhs.z);
        }

        const vec3f vec3f::operator * (float rhs) const {
            return vec3f(x * rhs, y * rhs, z * rhs);
        }

        const vec3f vec3f::operator / (float rhs) const {
            return *this * ((rhs == 0) ? 1 : 1/rhs);
        }

        vec3f& vec3f::operator += (vec3f const& rhs) {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        vec3f& vec3f::operator -= (vec3f const& rhs) {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }
#else
        template <typename D>
        vec3f::vec3f(templates::base<3,float,D> const& copy) {
            *this = copy;
        }

        template <typename D>
        vec3f& vec3f::operator = (templates::base<3,float,D> const& rhs) {
            D const& d = rhs.derived();
            x = d.eval(0);
            y = d.eval(1);
            z = d.eval(2);
            return *this;
        }

        template <typename D>
        vec3f& vec3f::operator += (templates::base<3,float,D> const& rhs) {
            D const& d = rhs.derived();
            x += d.eval(0);
            y += d.eval(1);
            z += d.eval(2);
            return *this;
        }

        template <typename D>
        vec3f& vec3f::operator -= (templates::base<3,float,D> const& rhs) {
            D const& d = rhs.derived();
            x -= d.eval(0);
            y -= d.eval(1);
            z -= d.eval(2);
            return *this;
        }
#endif

        vec3f& vec3f::operator *= (float rhs) {
            x *= rhs;
            y *= rhs;
            z *= rhs;
            return *this;
        }

        vec3f& vec3f::operator /= (float rhs) {
            return *this *= ((rhs == 0) ? 0 : 1/rhs);
        }

        void vec3f::normalize() {
            *this /= length();
        }

        float vec3f::length() const {
            return sqrt(x*x + y*y + z*z);
        }

        float vec3f::squareLength() const {
            return x*x + y*y + z*z;
        }

        float vec3f::dot(vec3f const& lhs, vec3f const& rhs) {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
        }

        const vec3f vec3f::cross(vec3f const& lhs, vec3f const& rhs) {
            return vec3f(
                lhs.y * rhs.z - lhs.z * rhs.y,
                lhs.z * rhs.x - lhs.x * rhs.z,
                lhs.x * rhs.y - lhs.y * rhs.x);
        }

        const float vec3f::operator [] (int index) const {
            return (&x)[index];
        }

        float& vec3f::operator [] (int index) {
            return (&x)[index];
        }

        float* vec3f::getRaw() {
            return &x;
        }

        float const* vec3f::getRaw() const {
            return &x;
        }

        bool vec3f::operator == (vec3f const& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }
    }
}

#endif