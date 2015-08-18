/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "blas.h"

#include <xmmintrin.h>
#include <memory>

namespace niwa {
    namespace math {
        namespace blas {
            Vector::Vector(int n, Initialization initialization) {
                data_ = static_cast<float*>(_mm_malloc(n * sizeof(float), 16));

                if(!data_) {
                    throw std::bad_alloc(); // ansi-iso compliant
                }

                if(initialization == Zero) {
                    szero(n, *this);
                }
            }

            Vector::~Vector() {
                _mm_free(data_);
            }

            float const* Vector::getRaw() const {
                return data_;
            }

            float* Vector::getRaw() {
                return data_;
            }

            void szero(int n, Vector& x) {
                int m = n/4;

                __m128* x128 = reinterpret_cast<__m128*>(x.getRaw());

                for(int i=0; i<m; ++i) {
                    x128[i] = _mm_set_ps1(0);
                }

                for(int i=m*4; i<n; ++i) {
                    x[i] = 0;
                }
            }

            void scopy(int n, Vector const& x, Vector& y) {
                int m = n/4;

                __m128 const* x128 = reinterpret_cast<__m128 const*>(x.getRaw());
                __m128* y128 = reinterpret_cast<__m128*>(y.getRaw());

                for(int i=0; i<m; ++i) {
                    y128[i] = x128[i];
                }

                for(int i=m*4; i<n; ++i) {
                    y[i] = x[i];
                }
            }

            void sscal(int n, float alpha, Vector& x) {
                int m = n/4;

                __m128* x128 = reinterpret_cast<__m128*>(x.getRaw());

                __m128 alpha128 = _mm_set_ps1(alpha);

                for(int i=0; i<m; ++i) {
                    x128[i] = _mm_mul_ps(x128[i], alpha128);
                }

                for(int i=m*4; i<n; ++i) {
                    x[i] *= alpha;
                }
            }

            void saxpy(int n, float alpha, Vector const& x, Vector& y) {
                int m = n/4;

                __m128 const* x128 = reinterpret_cast<__m128 const*>(x.getRaw());
                __m128* y128 = reinterpret_cast<__m128*>(y.getRaw());

                __m128 alpha128 = _mm_set_ps1(alpha);

                for(int i=0; i<m; ++i) {
                    y128[i] = _mm_add_ps(y128[i], _mm_mul_ps(x128[i], alpha128));
                }

                for(int i=m*4; i<n; ++i) {
                    y[i] += alpha * x[i];
                }
            }

            void sxpay(int n, Vector const& x, float alpha, Vector& y) {
                int m = n/4;

                __m128 const* x128 = reinterpret_cast<__m128 const*>(x.getRaw());
                __m128* y128 = reinterpret_cast<__m128*>(y.getRaw());

                __m128 alpha128 = _mm_set_ps1(alpha);

                for(int i=0; i<m; ++i) {
                    y128[i] = _mm_add_ps(_mm_mul_ps(alpha128, y128[i]), x128[i]);
                }

                for(int i=m*4; i<n; ++i) {
                    y[i] = alpha * y[i] + x[i];
                }
            }

            float sdot(int n, Vector const& x, Vector const& y) {
                int m = n/4;

                __m128 const* x128 = reinterpret_cast<__m128 const*>(x.getRaw());
                __m128 const* y128 = reinterpret_cast<__m128 const*>(y.getRaw());

                __m128 dot128 = _mm_set_ps1(0.0f);

                for(int i=0; i<m; ++i) {
                    dot128 = _mm_add_ps(dot128, _mm_mul_ps(x128[i], y128[i]));
                }
              
                float dot = 0;

                for(int i=0; i<4; ++i) {
                    dot += dot128.m128_f32[i];
                }

                for(int i=m*4; i<n; ++i) {
                    dot += x[i] * y[i];
                }

                return dot;
            }
        }
    }
}