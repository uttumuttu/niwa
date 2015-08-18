/**
* @file
* @author Mikko Kauppila
* 
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_MATH_FASTMATH_INL
#define NIWA_MATH_FASTMATH_INL

namespace niwa {
    namespace math {
        __forceinline float rsqrt_nr(float number) {
            const float threehalfs = 1.5F;

            float x2 = number * 0.5F;
            float y  = number;
            long i  = * ( long * ) &y;  // evil floating point bit level hacking
            i  = 0x5f3759df - ( i >> 1 ); // what the fuck?
            y  = * ( float * ) &i;
            y  = y * ( threehalfs - ( x2 * y * y ) ); // 1st iteration
            // y  = y * ( threehalfs - ( x2 * y * y ) ); // 2nd iteration, this can be removed

            return y;
        }
    }
}

#endif