/**
 * @file 
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_SYSTEM_SINGLETHREADEDPARALLELIZER_H
#define NIWA_SYSTEM_SINGLETHREADEDPARALLELIZER_H

#include "IParallelizer.h"

namespace niwa {
    namespace system {
        /**
         * A (non-)parallelizer that uses the calling
         * thread for non-parallel looping.
         */
        class SingleThreadedParallelizer : public IParallelizer {
        public:
            void loop(ICallback& callback, int start, int end);

            void loop(ICallback& callback, int start, int end, int stride);

            static SingleThreadedParallelizer* create();

        private: // prevent copying
            SingleThreadedParallelizer(SingleThreadedParallelizer const&);
            SingleThreadedParallelizer& operator = (SingleThreadedParallelizer const&);

        private:
            SingleThreadedParallelizer();
        };
    }
}

#endif