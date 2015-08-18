/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RANDOM_RANDOMSET_H
#define NIWA_RANDOM_RANDOMSET_H

#include "niwa/system/SpinLock.h"

namespace niwa {
    namespace random {
        class IRandom;

        /**
         * A set of random number generators.
         * Provides optional thread-safety via spin-locking.
         */
        class RandomSet {
        public:
            /**
             * @param dimension Dimension of the random set.
             * @param components The components of the set, ownership is transfered.
             * @param isThreadSafe Whether the set needs to be thread-safe.
             */
            RandomSet(int dimension, IRandom* const * const components, bool isThreadSafe);

            ~RandomSet();

            int __fastcall dimension();

            void __fastcall setSeed(long seed);

            /**
             * @param values An array with the same dimension as the set.
             */
            void __fastcall nextf(float* values);

            /**
             * @param values An array with the same dimension as the set.
             */
            void __fastcall nextd(double* values);

        private: // prevent copying
            RandomSet(RandomSet const&);
            RandomSet& operator = (RandomSet const&);

        private:
            const int dimension_;

            IRandom* const * const components_; // owned

            const bool isThreadSafe_;

            system::SpinLock lock_;
        };
    }
}

#endif
