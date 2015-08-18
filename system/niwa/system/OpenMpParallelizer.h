/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_SYSTEM_OPENMPPARALLELIZER_H
#define NIWA_SYSTEM_OPENMPPARALLELIZER_H

#include "IParallelizer.h"

namespace niwa {
    namespace system {
        /**
         * Redirects parallelization to OpenMP.
         */
        class OpenMpParallelizer : public IParallelizer {
        public:
            void loop(ICallback& callback, int start, int end);

            void loop(ICallback& callback, int start, int end, int stride);

            static OpenMpParallelizer* create();

            ~OpenMpParallelizer();

        private: // prevent copying
            OpenMpParallelizer(OpenMpParallelizer const&);
            OpenMpParallelizer& operator = (OpenMpParallelizer const&);

        private:
            OpenMpParallelizer();
        };
    }
}

#endif