/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/system/NiwaParallelizer.h"

#include "niwa/system/SingleThreadedParallelizer.h"
#include "niwa/system/ProcessorInfo.h"

#include "niwa/logging/Logger.h"

#include <winnt.h>

#include <algorithm>
#include <limits>
#include <cmath>

namespace {
    /**
     * Some MSVC distributions don't have InterlockedAdd,
     * so we'll define our own.
     */
    static __forceinline DWORD NiwaInterlockedAdd(LONG volatile* addend, LONG term) {
        LONG v;

        do {
            v = *addend;
        } while(v != InterlockedCompareExchange(addend, v+term, v));

        return v + term;
    }
}

namespace niwa {
    namespace system {
        static logging::Logger logger(typeid(NiwaParallelizer));

        class NiwaParallelizer::WorkerTask {
        public:
            WorkerTask(ICallback& callback, int start, int end, int stride);

            /**
             * @return The number of iterations completed.
             */
            long run();

        private: // prevent copying
            WorkerTask(WorkerTask const&);
            WorkerTask& operator = (WorkerTask const&);

        private:
            ICallback& callback_;

            LONG volatile position_;

            int end_;

            int stride_;
        };

        class NiwaParallelizer::Worker {
        public:
            Worker();
            ~Worker();

            /**
             * Second-phase construction.
             *
             * @return Whether the worker could be constructed.
             */
            bool construct(NiwaParallelizer* parent);

            void run();

            static DWORD WINAPI runStatic(void* worker);

            int getIterationsCompleted() const;
        private:
            NiwaParallelizer* parent_;

            HANDLE thread_;
            DWORD threadId_;

            long nIterationsCompleted_;
        };

        void NiwaParallelizer::loop(ICallback& callback, int start, int end) {
            loop(callback, start, end, 1);
        }

        void NiwaParallelizer::loop(ICallback& callback, int start, int end, int stride) {
            WorkerTask task(callback, start, end, stride);

            currentTask_ = &task;
            nActiveWorkers_ = nWorkers_;

            SetEvent(taskRunningEvent_);

            nIterationsCompleted_ += task.run(); // Let calling thread participate in task.

            WaitForSingleObject(taskCompletedEvent_, INFINITE);
            ResetEvent(taskCompletedEvent_);
        }

        IParallelizer* NiwaParallelizer::create() {
            NiwaParallelizer* self = new NiwaParallelizer();

            if(self->construct()) {
                return self;
            } else {
                delete self;
                return SingleThreadedParallelizer::create();
            }
        }

        NiwaParallelizer::NiwaParallelizer() 
            : taskRunningEvent_(0),
              taskCompletedEvent_(0),
              nIterationsCompleted_(0) {
            ProcessorInfo info = ProcessorInfo::create();

            nWorkers_ = info.getLogicalProcessorCount() - 1;

            workers_ = new NiwaParallelizer::Worker[nWorkers_];
        }

        NiwaParallelizer::~NiwaParallelizer() {
            // Run a poison pill task.

            currentTask_ = NULL;
            nActiveWorkers_ = nWorkers_;
            SetEvent(taskRunningEvent_);

            WaitForSingleObject(taskCompletedEvent_, INFINITE);
            ResetEvent(taskCompletedEvent_);

            // Log balance factor before deleting workers.
            logger.debug() << "balance factor: " << getBalanceFactor();

            delete[] workers_;

            if(taskRunningEvent_) {
                CloseHandle(taskRunningEvent_);
            }
            if(taskCompletedEvent_) {
                CloseHandle(taskCompletedEvent_);
            }
        }

        bool NiwaParallelizer::construct() {
            // Manual reset, initially not set.
            taskRunningEvent_ = CreateEvent(0, TRUE, FALSE, 0);

            // Manual reset, initially not set.
            taskCompletedEvent_ = CreateEvent(0, TRUE, FALSE, 0);

            for(int i=0; i<nWorkers_; ++i) {
                if(!workers_[i].construct(this)) {
                    return false;
                }
            }

            return taskRunningEvent_ && taskCompletedEvent_;
        }

        DWORD WINAPI NiwaParallelizer::Worker::runStatic(void* worker) {
            static_cast<Worker*>(worker)->run();
            return 0;
        }

        NiwaParallelizer::Worker::Worker() 
            : parent_(0), thread_(0),
              nIterationsCompleted_(0) {
            // ignored
        }

        bool NiwaParallelizer::Worker::construct(NiwaParallelizer* parent) {
            parent_ = parent;

            // It's best to create the thread last:
            // this way non-successful construction
            // implies that the thread is never started.
            thread_ = CreateThread(
                0, // security
                0, // stack size
                Worker::runStatic, // thread function
                this, // thread function argument
                0,
                &threadId_);

            return thread_ != 0;
        }

        NiwaParallelizer::Worker::~Worker() {
            if(thread_) {
                WaitForSingleObject(thread_, INFINITE);

                CloseHandle(thread_);
            }
        }

        void NiwaParallelizer::Worker::run() {
            bool isRunning = true;

            while(isRunning) {
                // TODO: this used to be WaitForSingleObject, but it lead to crashes
                // XXX: it's WaitForSingleObject again, since WaitForMultipleObjects didn't compile
                WaitForSingleObject(parent_->taskRunningEvent_, INFINITE); // error ignored

                if(parent_->currentTask_ != NULL) {
                    nIterationsCompleted_ += parent_->currentTask_->run();
                } else {
                    // poison pill
                    isRunning = false;
                }

                if( InterlockedDecrement(&parent_->nActiveWorkers_) == 0 ) {
                    parent_->currentTask_ = NULL;

                    ResetEvent(parent_->taskRunningEvent_);
                    SetEvent(parent_->taskCompletedEvent_);
                }
            }
        }

        NiwaParallelizer::WorkerTask::WorkerTask(ICallback& callback, int start, int end, int stride)
            : callback_(callback), position_(start - stride), end_(end), stride_(stride) {
            // ignored
        }

        long NiwaParallelizer::WorkerTask::run() {
            long nIterationsCompleted = 0;

            LONG next;

            while((next = NiwaInterlockedAdd(&position_,stride_)) < end_) {
                int jEnd = std::min<LONG>(end_, next + stride_);

                for(int j=next; j<jEnd; ++j) {
                    callback_.invoke(j);
                }

                ++nIterationsCompleted;
            }

            return nIterationsCompleted;
        }

        int NiwaParallelizer::Worker::getIterationsCompleted() const {
            return nIterationsCompleted_;
        }

        double NiwaParallelizer::getBalanceFactor() const {
            long nTotalIterations = nIterationsCompleted_;

            for(int i=0; i<nWorkers_; ++i) {
                nTotalIterations += workers_[i].getIterationsCompleted();
            }

            if(nTotalIterations == 0) {
                return std::numeric_limits<double>::quiet_NaN();
            }

            double entropy = 0;

            if(nIterationsCompleted_ != 0) {
                double p = 
                    nIterationsCompleted_ 
                    / static_cast<double>(nTotalIterations);

                entropy -= p * log(p);
            }

            for(int i=0; i<nWorkers_; ++i) {
                double p = workers_[i].getIterationsCompleted()
                    / static_cast<double>(nTotalIterations);

                if(p != 0) {
                    entropy -= p * log(p);
                }
            }

            entropy /= log(2.0); // Transform logarithms to base 2.

            double n = 1 + nWorkers_;

            double maxEntropy = log(n) / log(2.0);

            return entropy / maxEntropy;
        }
    }
}
