/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_PACKED_VEC3I_INL
#define NIWA_MATH_PACKED_VEC3I_INL

namespace niwa {
    namespace math {
#pragma warning(push)
#pragma warning(disable:4700)
        packed_vec3i packed_vec3i::createUninitialized() {
            // TODO: uninitialized version fails on debug; find a workaround.
            return packed_vec3i();
            //unsigned char space[sizeof(packed_vec3i)];
            //return *reinterpret_cast<packed_vec3i*>(space);
        }
#pragma warning(pop)

        packed_vec3i::packed_vec3i() 
            : x(_mm_setzero_ps()), y(_mm_setzero_ps()), z(_mm_setzero_ps()) {
            // ignored
        }

        packed_vec3i::packed_vec3i(vec3i const& vec)
            : x(_mm_set_ps1(*reinterpret_cast<float const*>(&vec.x))),
              y(_mm_set_ps1(*reinterpret_cast<float const*>(&vec.y))),
              z(_mm_set_ps1(*reinterpret_cast<float const*>(&vec.z))) {
            // ignored
        }

        packed_vec3i::packed_vec3i(
            __m128 const& x_, __m128 const& y_, __m128 const& z_) 
            : x(x_), y(y_), z(z_) {
            // ignored
        }

        void packed_vec3i::set(int i, vec3i const& vec) {
            x.m128_i32[i] = vec.x;
            y.m128_i32[i] = vec.y;
            z.m128_i32[i] = vec.z;
        }

        vec3i packed_vec3i::get(int i) const {
            return vec3i(x.m128_i32[i], y.m128_i32[i], z.m128_i32[i]);
        }
    }
}


#endif