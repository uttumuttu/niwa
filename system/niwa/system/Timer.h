/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_SYSTEM_TIMER_H
#define NIWA_SYSTEM_TIMER_H

#define NOMINMAX
#include <windows.h>

namespace niwa {
    namespace system {
        /**
         * A timer.
         */
        class Timer {
        public:
            Timer();

            /**
             * Starts the timer. Must be called
             * before calling measurements are made.
             * 
             * @return Whether the timer could be started.
             */
            bool start();

            /**
             * Measures the time in seconds.
             *
             * @param seconds If successful, contains the number of seconds since
             *                last measurement; or if this is the first call,
             *                the number of seconds since calling start.
             *                If not successful, contains zero.
             *
             * @return True if and only if the timer was started
             *         and the measurement was successful.
             */
            bool measureTime(double& seconds);

            /**
             * Measures lap time in seconds, and resets the timer.
             *
             * @param seconds If successful, contains the number of seconds since
             *                last call to measureLapTime; or if this is the 
             *                first call, the number of seconds since calling start.
             *                If not successful, contains zero.
             *
             * @return True if and only if the timer was started
             *         and the measurement was successful.
             */
            bool measureLapTime(double& seconds);

        private:
            bool measure(double& seconds, bool reset);

        private: // prevent copying
            Timer(Timer const&);
            Timer& operator = (Timer const&);

        private:
            bool isStarted_;

            double ticksPerSecond_;

            LARGE_INTEGER previousTicks_;
        };
    }
}

#endif