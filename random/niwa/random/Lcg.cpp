/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/random/Lcg.h"

namespace {
    static const double finalMultiplier = 1.0 / static_cast<double>(0x80000000);

    static niwa::random::Lcg globalInstance(true);
}

namespace niwa {
    namespace random {
        Lcg::Lcg(bool isThreadSafe) 
            : isThreadSafe_(isThreadSafe), value_(0) {
#ifndef NIWA_RANDOM_LCG_LOCK_FREE
            InitializeCriticalSection(&mutex_);
#endif
        }

        Lcg::~Lcg() {
#ifndef NIWA_RANDOM_LCG_LOCK_FREE
            DeleteCriticalSection(&mutex_);
#endif
        }

        void Lcg::setSeed(long seed) {
            if(isThreadSafe_) {
#ifdef NIWA_RANDOM_LCG_LOCK_FREE
                LONG backup;

                do {
                    backup = value_;
                } while(backup != InterlockedCompareExchange(&value_, seed, backup));
#else
                EnterCriticalSection(&mutex_);
                value_ = static_cast<unsigned int>(seed);
                LeaveCriticalSection(&mutex_);
#endif
            } else {
                value_ = static_cast<unsigned int>(seed);
            }
        }

        double Lcg::nextd() {
            LONG oldValue = value_;

            if(isThreadSafe_) {
#ifdef NIWA_RANDOM_LCG_LOCK_FREE
                LONG backup;
                LONG update;

                do {
                    backup = value_;
                    update = (1103515245 * backup + 12345) & 0xFFFFFFFF;
                } while(backup != InterlockedCompareExchange(&value_, update, backup));
#else
                EnterCriticalSection(&mutex_);
                value_ = (1103515245 * value_ + 12345) & 0xFFFFFFFF;
                LeaveCriticalSection(&mutex_);
#endif
            } else {
                LONG backup = value_;

                value_ = (1103515245 * backup + 12345) & 0xFFFFFFFF;
            }

            return (oldValue & 0x7FFFFFFF) * finalMultiplier;
        }

        float Lcg::nextf() {
            return static_cast<float>(nextd());
        }

        Lcg& Lcg::global() {
            return globalInstance;
        }
    }
}