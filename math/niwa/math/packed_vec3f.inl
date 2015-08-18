/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_PACKED_VEC3F_INL
#define NIWA_MATH_PACKED_VEC3F_INL

namespace niwa {
    namespace math {
#pragma warning(push)
#pragma warning(disable:4700)
        packed_vec3f packed_vec3f::createUninitialized() {
            // TODO: the uninitialized version fails with debugging; find a workaround.
            return packed_vec3f();
            //unsigned char space[sizeof(packed_vec3f)];
            //return *reinterpret_cast<packed_vec3f*>(space);
        }
#pragma warning(pop)

        packed_vec3f::packed_vec3f() 
            : x(_mm_setzero_ps()), y(_mm_setzero_ps()), z(_mm_setzero_ps()) {
            // ignored
        }

        packed_vec3f::packed_vec3f(vec3f const& vec) 
            : x(_mm_set_ps1(vec.x)), y(_mm_set_ps1(vec.y)), z(_mm_set_ps1(vec.z)) {
            // ignored
        }

        packed_vec3f::packed_vec3f(
            __m128 const& x_, __m128 const& y_, __m128 const& z_) 
            : x(x_), y(y_), z(z_) {
            // ignored
        }

        void packed_vec3f::set(int i, vec3f const& vec) {
            x.m128_f32[i] = vec.x;
            y.m128_f32[i] = vec.y;
            z.m128_f32[i] = vec.z;
        }

        vec3f packed_vec3f::get(int i) const {
            return vec3f(x.m128_f32[i], y.m128_f32[i], z.m128_f32[i]);
        }

        __m128 packed_vec3f::length() const {
            return _mm_sqrt_ps(
                _mm_add_ps(
                  _mm_mul_ps(x,x),
                  _mm_add_ps(
                    _mm_mul_ps(y,y),
                    _mm_mul_ps(z,z))));
        }

        __m128 packed_vec3f::squareLength() const {
            return _mm_add_ps(
                _mm_mul_ps(x,x),
                _mm_add_ps(
                  _mm_mul_ps(y,y),
                  _mm_mul_ps(z,z)));
        }

        const packed_vec3f packed_vec3f::operator + (packed_vec3f const& rhs) const {
            return packed_vec3f(
                _mm_add_ps(x, rhs.x),
                _mm_add_ps(y, rhs.y),
                _mm_add_ps(z, rhs.z));
        }

        const packed_vec3f packed_vec3f::operator - (packed_vec3f const& rhs) const {
            return packed_vec3f(
                _mm_sub_ps(x, rhs.x),
                _mm_sub_ps(y, rhs.y),
                _mm_sub_ps(z, rhs.z));
        }

        const packed_vec3f packed_vec3f::operator * (__m128 const& rhs) const {
            return packed_vec3f(
                _mm_mul_ps(x, rhs),
                _mm_mul_ps(y, rhs),
                _mm_mul_ps(z, rhs));
        }

        const __m128 packed_vec3f::dot(packed_vec3f const& rhs) const {
            return _mm_add_ps(
                _mm_mul_ps(x, rhs.x),
                _mm_add_ps(
                  _mm_mul_ps(y, rhs.y),
                  _mm_mul_ps(z, rhs.z)));
        }

        packed_vec3f& packed_vec3f::operator *= (__m128 const& rhs) {
            x = _mm_mul_ps(x, rhs);
            y = _mm_mul_ps(y, rhs);
            z = _mm_mul_ps(z, rhs);
            return *this;
        }

        packed_vec3f& packed_vec3f::operator /= (__m128 const& rhs) {
            return *this *= _mm_rcp_ps(rhs);
        }

        void packed_vec3f::normalizeApproximate() {
            *this *= _mm_rsqrt_ps(squareLength());
        }

        void packed_vec3f::normalizeAccurate() {
            __m128 x = length();
            for(int i=0; i<4; ++i) {
                x.m128_f32[i] = 1 / x.m128_f32[i];
            }
            *this *= x;
        }
    }
}


#endif