/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_SYSTEM_NIWAPARALLELIZER_H
#define NIWA_SYSTEM_NIWAPARALLELIZER_H

#define NOMINMAX
#include <windows.h>

#include "IParallelizer.h"

namespace niwa {
    namespace system {
        /**
         * A proprietary parallelizer developed for niwa.
         * Creates N-1 threads, where N is the number of
         * logical processors. Uses atomic integers for
         * shared loop indexing. For Windows only.
         */
        class NiwaParallelizer : public IParallelizer {
        public:
            void loop(ICallback& callback, int start, int end);

            void loop(ICallback& callback, int start, int end, int stride);

            /**
             * @return The parallelizer. If the parallelizer cannot be
             *         created, returns a single-threaded parallelizer.
             */
            static IParallelizer* create();

            /**
             * Gets the balance factor (balancing
             * entropy relative to maximum entropy).
             *
             * @return A value between zero and one (inclusive)
             *         that specifies how well work is balanced
             *         between processors. If the parallelizer
             *         has not run any work yet, the value is NaN.
             */
            double getBalanceFactor() const;

            ~NiwaParallelizer();
        private:
            NiwaParallelizer();

            /**
             * Second-phase construction.
             *
             * @return Whether the parallelizer could be constructed.
             */
            bool construct();

            class WorkerTask;

            class Worker;

        private: // prevent copying
            NiwaParallelizer(NiwaParallelizer const&);
            NiwaParallelizer& operator = (NiwaParallelizer const&);

        private:
            int nWorkers_;

            HANDLE taskRunningEvent_;
            HANDLE taskCompletedEvent_;

            /**
             * A rendezvous variable: once
             * the last worker decrements it to zero,
             * the task is set complete.
             */
            LONG volatile nActiveWorkers_;

            /**
             * Current task, or NULL for a poison pill
             * that stops all workers.
             */
            WorkerTask* currentTask_;

            Worker* workers_; // owned

            /**
             * The number of work iterations completed
             * by the calling thread.
             */
            long nIterationsCompleted_;
        };
    }
}

#endif