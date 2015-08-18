/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RANDOM_LCG_H
#define NIWA_RANDOM_LCG_H

#include "IRandom.h"

#define NOMINMAX
#include <windows.h>

#define NIWA_RANDOM_LCG_LOCK_FREE

namespace niwa {
    namespace random {
        /**
         * Linear congruential generator (LCG).
         * Uses the same parameters as
         * the rand() function in GNU GCC.
         */
        class Lcg : public IRandom {
        public:
            /**
             * Creates a LCG with zero seed.
             *
             * @param isThreadSafe Whether the LCG must be thread-safe.
             */
            Lcg(bool isThreadSafe);

            ~Lcg();

        public: // from IRandom
            /**
             * Only the first 32 bits of the seed are used.
             */
            void __fastcall setSeed(long seed);

            double __fastcall nextd();

            float __fastcall nextf();

        public: // new functions
            /**
             * @return A global, thread-safe LCG.
             */
            static Lcg& global();
        private:
            bool isThreadSafe_;

#ifdef NIWA_RANDOM_LCG_LOCK_FREE
            LONG volatile value_;
#else
            LONG value_;

            CRITICAL_SECTION mutex_;
#endif
        };
    }
}

#endif