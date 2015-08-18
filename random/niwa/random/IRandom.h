/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RANDOM_IRANDOM_H
#define NIWA_RANDOM_IRANDOM_H

namespace niwa {
    namespace random {
        /**
         * Random number generator.
         */
        class IRandom {
        public:
            virtual ~IRandom();

            /**
             * Reinitializes the sequence
             * with the given seed.
             */
            virtual void __fastcall setSeed(long seed) = 0;

            /**
             * @return Uniformly distributed random
             *         number in range [0,1).
             */
            virtual double __fastcall nextd() = 0;

            /**
             * @return Uniformly distributed random
             *         number in range [0,1).
             */
            virtual float __fastcall nextf() = 0;
        };
    }
}

#endif