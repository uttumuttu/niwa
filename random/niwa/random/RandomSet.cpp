/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/random/RandomSet.h"

#include "niwa/random/IRandom.h"

#include "niwa/system/ScopedAcquire.h"

namespace niwa {
    namespace random {
        using niwa::system::ScopedAcquire;

        RandomSet::RandomSet(int dimension, IRandom* const * const components, bool isThreadSafe) 
            : dimension_(dimension), components_(components), isThreadSafe_(isThreadSafe) {
            // ignored
        }

        RandomSet::~RandomSet() {
            for(int i=0; i<dimension_; ++i) {
                delete components_[i];
            }
            delete[] components_;
        }

        int RandomSet::dimension() {
            return dimension_;
        }

        void RandomSet::setSeed(long seed) {
            ScopedAcquire acquire(isThreadSafe_ ? &lock_ : NULL);

            for(int i=0; i<dimension_; ++i) {
                components_[i]->setSeed(seed);
            }
        }

        void RandomSet::nextf(float* values) {
            ScopedAcquire acquire(isThreadSafe_ ? &lock_ : NULL);

            for(int i=0; i<dimension_; ++i) {
                values[i] = components_[i]->nextf();
            }
        }

        void RandomSet::nextd(double* values) {
            ScopedAcquire acquire(isThreadSafe_ ? &lock_ : NULL);

            for(int i=0; i<dimension_; ++i) {
                values[i] = components_[i]->nextd();
            }
        }
    }
}