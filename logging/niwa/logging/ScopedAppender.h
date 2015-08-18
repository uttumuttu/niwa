/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_LOGGING_SCOPEDAPPENDER_H
#define NIWA_LOGGING_SCOPEDAPPENDER_H

namespace niwa {
    namespace logging {
        class IAppender;

        /**
         * RAII wrapper for adding an appender
         * to the logging system.
         */
        class ScopedAppender {
        public:
            /**
             * Adds the appender to the logging system.
             *
             * @param appender Must not be NULL.
             */
            __declspec(dllexport) explicit ScopedAppender(IAppender* appender);

            /**
             * Removes the appender from the logging system.
             */
            __declspec(dllexport) ~ScopedAppender();

        private:
            IAppender* appender_;
        };
    }
}

#endif