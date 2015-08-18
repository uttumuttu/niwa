/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_CONSTANTS_H
#define NIWA_RAYTRACE_CONSTANTS_H

#include <xmmintrin.h>

namespace niwa {
    namespace raytrace {
        namespace constants {
            static const float DISTANCE_EPSILON = 1e-3f;

            __declspec(align(16)) 
            static const __m128 PACKED_DISTANCE_EPSILON = _mm_set_ps1(DISTANCE_EPSILON);
        }
    }
}

#endif