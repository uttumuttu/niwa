/**
 * @file 
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_RAY3F_INL
#define NIWA_RAYTRACE_RAY3F_INL

/**
* SSE's reciprocal operation is approximative.
* The approximation may be too inaccurate for
* some applications. By switching this variable
* to zero, a non-approximate reciprocal will be used.
*/
#define NIWA_RAYTRACE_RAY3F_SSE_RCP 1

namespace niwa {
    namespace raytrace {
        ray3f::ray3f(math::vec3f const& position, math::vec3f const& direction) {
            *reinterpret_cast<math::vec3f*>(&position_) = position;
            *reinterpret_cast<math::vec3f*>(&direction_) = direction;

            __declspec(align(16)) static int const ones[4] = {1,1,1,1};

            signs_ = _mm_and_ps(
                _mm_cmplt_ps(direction_, _mm_setzero_ps()),
                *reinterpret_cast<__m128 const*>(ones));

#if NIWA_RAYTRACE_RAY3F_SSE_RCP
            directionInverses_ = _mm_rcp_ps(direction_);
#else
            for(int i=0; i<3; ++i) {
                directionInverses_.m128_f32[i] = 1 / direction_.m128_f32[i];
            }
#endif
        }

        math::vec3f const& ray3f::getPosition() const {
            return *reinterpret_cast<math::vec3f const*>(&position_);
        }

        math::vec3f const& ray3f::getDirection() const {
            return *reinterpret_cast<math::vec3f const*>(&direction_);
        }

        math::vec3i const& ray3f::getSigns() const {
            return *reinterpret_cast<math::vec3i const*>(&signs_);
        }

        math::vec3f const& ray3f::getDirectionInverses() const {
            return *reinterpret_cast<math::vec3f const*>(&directionInverses_);
        }

        ray3f::ray3f(
            math::vec3f const& position, math::vec3f const& direction,
            math::vec3i const& signs, math::vec3f const& directionInverses) {
            *reinterpret_cast<math::vec3f*>(&position_) = position;
            *reinterpret_cast<math::vec3f*>(&direction_) = direction;
            *reinterpret_cast<math::vec3i*>(&signs_) = signs;
            *reinterpret_cast<math::vec3f*>(&directionInverses_) = directionInverses;
        }
    }
}

#endif