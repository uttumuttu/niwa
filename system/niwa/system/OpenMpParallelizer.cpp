/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/system/OpenMpParallelizer.h"

namespace niwa {
    namespace system {
        OpenMpParallelizer::OpenMpParallelizer() {
            // ignored
        }

        OpenMpParallelizer::~OpenMpParallelizer() {
            // ignored
        }

        OpenMpParallelizer* OpenMpParallelizer::create() {
            return new OpenMpParallelizer();
        }

        void OpenMpParallelizer::loop(ICallback& callback, int start, int end) {
#pragma omp parallel for
            for(int i=start; i<end; ++i) {
                callback.invoke(i);
            }
        }

        void OpenMpParallelizer::loop(ICallback& callback, int start, int end, int /*stride*/) {
#pragma omp parallel for
            for(int i=start; i<end; ++i) {
                callback.invoke(i);
            }
        }
    }
}