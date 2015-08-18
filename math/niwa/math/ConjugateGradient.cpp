/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "ConjugateGradient.h"

#include "Blas.h"

namespace niwa {
    namespace math {
        using namespace blas;

        ConjugateGradient::ConjugateGradient(
                int maxIterations,
                float epsilon)
            : maxIterations_(maxIterations), epsilon_(epsilon) {
            // ignored
        }

        /**
         * After Shewchuk's paper
         * "Introduction to The Conjugate Gradient Method
         * Without The Agonizing Pain".
         */
        void ConjugateGradient::solve(
                int n,
                void (*op)(Vector const&, Vector &, void*),
                void* arg,
                Vector const& b,
                Vector& x) {
            Vector r(n, Vector::Uninitialized);
            Vector d(n, Vector::Uninitialized);
            Vector q(n, Vector::Uninitialized);

            op(x, r, arg);
            
            sxpay(n, b, -1, r);

            scopy(n, r, d);

            float errorNew = sdot(n, r, r);

            float errorZero = errorNew;

            for(int i=0; i<maxIterations_ && errorNew > epsilon_*epsilon_*errorZero; ++i) {
                op(d, q, arg);

                float denominator = sdot(n, d, q);

                if(denominator == 0) {
                    break;
                }

                float alpha = errorNew / denominator;

                saxpy(n, alpha, d, x);

                if(i > 0 && (i%50) == 0) {
                    op(x, r, arg);

                    sxpay(n, b, -1, r);
                } else {
                    saxpy(n, -alpha, q, r);
                }

                float errorOld = errorNew;

                errorNew = sdot(n, r, r);

                float beta = errorNew / errorOld;

                sxpay(n, r, beta, d);
            }
        }
    }
}