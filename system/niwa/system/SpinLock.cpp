/**
 * @file 
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/system/SpinLock.h"

namespace niwa {
    namespace system {
        SpinLock::SpinLock() : isLocked_(0) {
            // ignored
        }

        void SpinLock::acquire() {
            while(InterlockedCompareExchange(&isLocked_, 1, 0));
        }

        void SpinLock::release() {
            InterlockedExchange(&isLocked_, 0);
        }
    }
}