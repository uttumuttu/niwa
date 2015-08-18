/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RANDOM_VANDERCORPUT_H
#define NIWA_RANDOM_VANDERCORPUT_H

#include "IRandom.h"

namespace niwa {
    namespace random {
        /**
         * Van der Corput sequence: a specialized fixed-point
         * implementation of base-2 Halton sequence.
         *
         * @todo Insulate, but beware of losses in efficiency
         *       associated with virtual functions and __fastcall.
         */
        class VanDerCorput {
        public:
            VanDerCorput() : value_(0) {
                // ignored
            }

        public: // from IRandom
            /**
             * Does not initialize the sequence to the
             * nth Van der Corput value, but uses a more
             * efficient version with an equivalent distribution
             * (as long as the first 32 bits of the seed are
             * distributed uniformly).
             */
            void setSeed(long seed) {
                value_ = seed & 0xFFFFFFFFU;
            }

            float nextf() {
                return static_cast<float>(nextd());
            }

            double nextd() {
                unsigned int backup = value_;

                value_ = next(value_);

                return backup / 4294967296.0;
            }

        private:
            static unsigned int next(unsigned long value) {
                const unsigned int residual = 0xFFFFFFFFU - value;

                const unsigned int invBase = 0x100000000L >> 1;

                if(invBase < residual) {
                    value += invBase;
                } else {
                    unsigned int h = invBase;
                    unsigned int hh;

                    do{
                        hh = h;
                        h >>= 1;
                    } while ( h >= residual );

                    value += hh + h;
                }

                return value;
            }
        private:
            unsigned int value_;
        };
    }
}

#endif