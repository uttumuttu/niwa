/**
 * @file 
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/system/SingleThreadedParallelizer.h"

#include <algorithm>

namespace niwa {
    namespace system {
        void SingleThreadedParallelizer::loop(ICallback& callback, int start, int end) {
            for(int i=start; i<end; ++i) {
                callback.invoke(i);
            }
        }

        void SingleThreadedParallelizer::loop(ICallback& callback, int start, int end, int stride) {
            for(int i=start; i<end; i+=stride) {
                int jEnd = std::min(end, i + stride);

                for(int j=i; j<jEnd; ++j) {
                    callback.invoke(j);
                }
            }
        }

        SingleThreadedParallelizer* SingleThreadedParallelizer::create() {
            return new SingleThreadedParallelizer();
        }

        SingleThreadedParallelizer::SingleThreadedParallelizer() {
            // ignored
        }
    }
}