/**
 * @file 
 * @author Mikko Kauppila
 * 
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RANDOM_EVENLYSPACEDSEQUENCE_H
#define NIWA_RANDOM_EVENLYSPACEDSEQUENCE_H

#include "IRandom.h"

namespace niwa {
    namespace random {
        /**
         * An evenly-spaced quasi-random sequence.
         *
         * The implementation is inlined, since
         * inlining gives an extra boost on this kind
         * of simple sequences.
         */
        class EvenlySpacedSequence : public niwa::random::IRandom {
        public:
            /**
             * @param length Length of the sequence.
             *
             * @param offset In range [0,1).
             */
            EvenlySpacedSequence(int length, double offset);

            __forceinline void __fastcall setSeed(long seed);

            __forceinline double __fastcall nextd();

            __forceinline float __fastcall nextf();

        private: // prevent copying
            EvenlySpacedSequence(EvenlySpacedSequence const&);
            EvenlySpacedSequence& operator = (EvenlySpacedSequence const&);

        private:
            const int length_;

            const float invLengthFloat_; // auxiliary
            const double invLengthDouble_; // auxiliary

            const float offsetFloat_;
            const double offsetDouble_;

            int position_;
        };
    }
}

#include "EvenlySpacedSequence.inl"

#endif