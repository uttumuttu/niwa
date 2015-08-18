/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_BLAS_H
#define NIWA_MATH_BLAS_H

namespace niwa {
    namespace math {
        /**
         * Basic linear algebra subprograms.
         */
        namespace blas {
            class Vector {
            public:
                enum Initialization {
                    Zero,
                    Uninitialized
                };

            public:
                Vector(int n, Initialization initialization);

                ~Vector();

                __forceinline float operator[] (int index) const;

                __forceinline float& operator[] (int index);

                float const* getRaw() const;

                float* getRaw();

            private: // prevent copying
                Vector(Vector const&);
                Vector& operator = (Vector const&);

            private:
                float* data_; // owned
            };

            void szero(int n, Vector& x);

            void scopy(int n, Vector const& x, Vector& y);

            void sscal(int n, float alpha, Vector& x);

            void saxpy(int n, float alpha, Vector const& x, Vector& y);

            void sxpay(int n, Vector const& x, float alpha, Vector& y);

            float sdot(int n, Vector const& x, Vector const& y);
        }
    }
}

#include "Blas.inl"

#endif