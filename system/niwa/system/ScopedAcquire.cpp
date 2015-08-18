/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "ScopedAcquire.h"

#include "ILock.h"

namespace niwa {
    namespace system {
        ScopedAcquire::ScopedAcquire(ILock* lock) 
            : lock_(lock) {
            if(lock_) {
                lock_->acquire();
            }
        }

        ScopedAcquire::~ScopedAcquire() {
            if(lock_) {
                lock_->release();
            }
        }
    }
}
