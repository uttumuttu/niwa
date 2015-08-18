/**
 * @file 
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_PACKED_RAY3F_INL
#define NIWA_RAYTRACE_PACKED_RAY3F_INL

/**
 * SSE's reciprocal operation is approximative.
 * The approximation may be too inaccurate for
 * some applications. By switching this variable
 * to zero, a non-approximate reciprocal will be used.
 */
#define NIWA_RAYTRACE_PACKED_RAY3F_SSE_RCP 1

namespace niwa {
    namespace raytrace {
        packed_ray3f::packed_ray3f() {
            // ignored
        }

        packed_ray3f::packed_ray3f(
            math::packed_vec3f const& position,
            math::packed_vec3f const& direction)
            : position_(position), direction_(direction),
              signs_(math::packed_vec3i::createUninitialized()),
              directionInverses_(math::packed_vec3f::createUninitialized()) {
            __declspec(align(16)) int const ones[4] = {1,1,1,1};

            __m128 const& ones128 = *reinterpret_cast<__m128 const*>(ones);

            __m128* dir = reinterpret_cast<__m128*>(&direction_);
            __m128* sgn = reinterpret_cast<__m128*>(&signs_);
            __m128* inv = reinterpret_cast<__m128*>(&directionInverses_);

            for(size_t i=0; i<3; ++i) {
                sgn[i] = _mm_and_ps(ones128, _mm_cmplt_ps(dir[i], _mm_setzero_ps()));
            }

#if NIWA_RAYTRACE_PACKED_RAY3F_SSE_RCP
            for(size_t i=0; i<3; ++i) {
                inv[i] = _mm_rcp_ps(dir[i]);
            }
#else
            // Masks.
            __m128 mx = _mm_cmpneq_ps(_mm_setzero_ps(), dir[0]);
            __m128 my = _mm_cmpneq_ps(_mm_setzero_ps(), dir[1]);
            __m128 mz = _mm_cmpneq_ps(_mm_setzero_ps(), dir[2]);

            // Multiplication-safe directions (zeros replaced with ones).
            __m128 dx = _mm_or_ps(_mm_and_ps(mx, dir[0]), _mm_andnot_ps(mx, ones128));
            __m128 dy = _mm_or_ps(_mm_and_ps(my, dir[1]), _mm_andnot_ps(my, ones128));
            __m128 dz = _mm_or_ps(_mm_and_ps(mz, dir[2]), _mm_andnot_ps(mz, ones128));

            __m128 xyz = _mm_mul_ps(dx, _mm_mul_ps(dy, dz));

            for(size_t j=0; j<4; ++j) {
                xyz.m128_f32[j] = 1 / xyz.m128_f32[j];
            }

            inv[0] = _mm_mul_ps(_mm_mul_ps(dir[1], dir[2]), xyz);
            inv[1] = _mm_mul_ps(_mm_mul_ps(dir[0], dir[2]), xyz);
            inv[2] = _mm_mul_ps(_mm_mul_ps(dir[0], dir[1]), xyz);
#endif
        }

        void packed_ray3f::set(int index, ray3f const& ray) {
            position_.set(index, ray.getPosition());
            direction_.set(index, ray.getDirection());

            signs_.set(index, ray.getSigns());
            directionInverses_.set(index, ray.getDirectionInverses());
        }

        ray3f packed_ray3f::get(int index) const {
            return ray3f(
                position_.get(index), direction_.get(index),
                signs_.get(index), directionInverses_.get(index));
        }

        math::packed_vec3f const& packed_ray3f::getPosition() const {
            return position_;
        }

        math::packed_vec3f const& packed_ray3f::getDirection() const {
            return direction_;
        }

        math::packed_vec3i const& packed_ray3f::getSigns() const {
            return signs_;
        }

        math::packed_vec3f const& packed_ray3f::getDirectionInverses() const {
            return directionInverses_;
        }
    }
}

#endif