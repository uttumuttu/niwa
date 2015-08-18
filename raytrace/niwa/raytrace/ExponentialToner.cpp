/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/ExponentialToner.h"

#include <cmath>

#include <xmmintrin.h>

/**
 * Whether to use a rational
 * approximation of exponential function
 * (computed on SSE) instead of accurate
 * exponential (computed without SSE).
 */
#define USE_APPROXIMATION 1

namespace niwa {
    using graphics::Spectrum;

     namespace raytrace {
        ExponentialToner::ExponentialToner() : strength_(1) {
            // ignored
        }

        void ExponentialToner::setStrength(float strength) {
            strength_ = strength;
        }

        const Spectrum ExponentialToner::toneMap(Spectrum const& irradiation) const {
#if USE_APPROXIMATION
            __m128 x = _mm_mul_ps(
                _mm_setr_ps(
                    irradiation.r, irradiation.g, irradiation.b, 0),
                _mm_set_ps1(strength_));

            __m128 e = _mm_add_ps(
                _mm_set_ps1(1.0f),
                _mm_mul_ps(x, _mm_set_ps1(1/16.0f)));

            e = _mm_mul_ps(e,e);
            e = _mm_mul_ps(e,e);
            e = _mm_mul_ps(e,e);
            e = _mm_mul_ps(e,e);

            __m128 y = _mm_sub_ps(_mm_set_ps1(1.0f), _mm_rcp_ps(e));

            return Spectrum(y.m128_f32);
#else
            return Spectrum(
                1 - expf(-irradiation.r * strength_),
                1 - expf(-irradiation.g * strength_),
                1 - expf(-irradiation.b * strength_));
#endif
        }
    }
}