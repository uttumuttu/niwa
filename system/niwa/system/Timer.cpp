/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/system/Timer.h"

namespace niwa {
    namespace system {
        Timer::Timer() : isStarted_(false) {
            // ignored
        }

        bool Timer::start() {
            LARGE_INTEGER temp;

            if(!isStarted_
                && QueryPerformanceFrequency(&temp)
                && QueryPerformanceCounter(&previousTicks_)) {
                isStarted_ = true;

                ticksPerSecond_ = static_cast<double>(temp.QuadPart);

                return true;
            } else {
                return false;
            }
        }

        bool Timer::measureTime(double& seconds) {
            return measure(seconds, false);
        }

        bool Timer::measureLapTime(double& seconds) {
            return measure(seconds, true);
        }

        bool Timer::measure(double& seconds, bool reset) {
            if(!isStarted_) {
                seconds = 0;
                return false;
            }

            LARGE_INTEGER currentTicks;

            if(QueryPerformanceCounter(&currentTicks)) {
                seconds = (currentTicks.QuadPart - previousTicks_.QuadPart) 
                        / ticksPerSecond_;

                if(reset) {
                    previousTicks_ = currentTicks;
                }
                return true;
            } else {
                seconds = 0;
                return false;
            }
        }
    }
}