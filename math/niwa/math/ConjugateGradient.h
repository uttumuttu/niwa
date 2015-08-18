/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_CONJUGATEGRADIENT_H
#define NIWA_MATH_CONJUGATEGRADIENT_H

namespace niwa {
    namespace math {
        namespace blas {
            class Vector;
        }

        class ConjugateGradient {
        public:
            /**
             * @param epsilon A value less than one that describes
             *                how much L2 norm is left in the final residual
             *                relative to the initial residual.
             */
            ConjugateGradient(int maxIterations, float epsilon);

            /**
             * @param n Problem dimensions.
             *
             * @param op Linear operator (implicit n-by-n matrix multiplication).
             *
             * @param b Right-hand side column n-vector.
             *
             * @param x Resulting column n-vector; initially contains the
             *          initial guess (e.g., zero).
             */
            void solve(
                int n,
                void (*op)(blas::Vector const&, blas::Vector&, void* arg),
                void* arg,
                blas::Vector const& b,
                blas::Vector& x);

        private:
            int maxIterations_;

            float epsilon_;
        };
    }
}

#endif