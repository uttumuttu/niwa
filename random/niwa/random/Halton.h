/**
 * @file 
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RANDOM_HALTON_H
#define NIWA_RANDOM_HALTON_H

#include "IRandom.h"

namespace niwa {
    namespace random {
        class RandomSet;

        /**
         * Halton quasi-random sequence generator.
         * 
         * Implementation is based on a very fast
         * algorithm given in the paper
         * The Fast Calculation of Form Factors Using Low Discrepancy Sequences 
         * (Alexander Keller, SCCG'96, pages 195–-204).
         *
         * The implementation isn't inlined: inlining doesn't
         * seem to give any extra boost.
         */
        class Halton : public IRandom {
        public:
            /**
             * Creates a Halton sequence with zero seed.
             *
             * @param base Greater than one, preferably a prime.
             */
            explicit Halton(int base);

            void __fastcall setSeed(long seed);

            double __fastcall nextd();

            float __fastcall nextf();

            /**
             * Creates a Halton set of a given dimension.
             */
            static RandomSet* createHaltonSet(int dimension, bool isThreadSafe);

            /**
             * Creates a Halton-Hammersley set of a given dimension
             * and sample count.
             *
             * @param dimension Dimension of the set.
             * @param length Length of the sequence.
             * @param isThreadSafe Whether the set must be thread-safe.
             */
            static RandomSet* createHaltonHammersleySet(int dimension, int length, bool isThreadSafe);

        private: // prevent copying
            Halton(Halton const&);
            Halton& operator = (Halton const&);

        private:
            /**
             * Current sequence value.
             */
            double value_;   

            const int base_;

            /**
             * Inverse of the base (auxiliary).
             */
            const double invBase_;
        };
    }
}

#endif