/**
 * @file 
 * @author Mikko Kauppila
 * 
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/random/EvenlySpacedSequence.h"

namespace niwa {
    namespace random {
        EvenlySpacedSequence::EvenlySpacedSequence(int length, double offset)
            : length_(length), invLengthDouble_(1.0 / length), invLengthFloat_(1.0f / length),
              offsetDouble_(offset / length), offsetFloat_(static_cast<float>(offset) / length),
              position_(0) {
            // ignored
        }
    }
}

