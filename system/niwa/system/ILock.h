/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_SYSTEM_ILOCK_H
#define NIWA_SYSTEM_ILOCK_H

namespace niwa {
    namespace system {
        class ILock {
        public:
            virtual ~ILock();

            virtual void acquire() = 0;

            virtual void release() = 0;
        };
    }
}

#endif