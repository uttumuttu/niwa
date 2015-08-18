/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_SYSTEM_SPINLOCK_H
#define NIWA_SYSTEM_SPINLOCK_H

#include "ILock.h"

#define NOMINMAX
#include <windows.h>

namespace niwa {
    namespace system {
        /**
         * A non-reentrant spin lock.
         */
        class SpinLock : public ILock {
        public:
            SpinLock();

            void acquire();

            void release();

        private: // prevent copying
            SpinLock(SpinLock const&);
            SpinLock& operator = (SpinLock const&);

        private:
            LONG volatile isLocked_;
        };
    }
}

#endif