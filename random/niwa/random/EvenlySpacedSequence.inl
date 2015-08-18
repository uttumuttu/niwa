/**
* @file 
* @author Mikko Kauppila
* 
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_RANDOM_EVENLYSPACEDSEQUENCE_INL
#define NIWA_RANDOM_EVENLYSPACEDSEQUENCE_INL

namespace niwa {
    namespace random {
        void EvenlySpacedSequence::setSeed(long seed) {
            position_ = seed % length_;
        }

        double EvenlySpacedSequence::nextd() {
            double result = (position_++) * invLengthDouble_;

            if(position_ >= length_) {
                position_ = 0;
            }
            return result + offsetDouble_;
        }

        float EvenlySpacedSequence::nextf() {
            float result = (position_++) * invLengthFloat_;

            if(position_ >= length_) {
                position_ = 0;
            }
            return result + offsetFloat_;
        }
    }
}

#endif