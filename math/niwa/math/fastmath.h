/**
 * @file
 * @author Mikko Kauppila
 * 
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_FASTMATH_H
#define NIWA_MATH_FASTMATH_H

namespace niwa {
    namespace math {
        /**
         * One-iteration Newton--Raphson approximation
         * of reciprocal square root.
         */
        static __forceinline float rsqrt_nr(float number);
    }
}

#include "fastmath.inl"

#endif