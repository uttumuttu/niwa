/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_SYSTEM_SCOPEDACQUIRE_H
#define NIWA_SYSTEM_SCOPEDACQUIRE_H

namespace niwa {
    namespace system {
        class ILock;

        /**
         * RAII acquirement of lock.
         */
        class ScopedAcquire {
        public:
            /**
             * Acquires the lock, if not NULL.
             *
             * @param lock 
             *        The lock. If NULL, no acquirement 
             *      (in constructor) or release
             *      (in destructor) is done.
             */
            explicit ScopedAcquire(ILock* lock);

            /**
             * Releases the lock, if not NULL.
             */
            ~ScopedAcquire();

        private: // prevent copying
            ScopedAcquire(ScopedAcquire const&);
            ScopedAcquire& operator = (ScopedAcquire const&);

        private:
            ILock* lock_;
        };
    }
}

#endif