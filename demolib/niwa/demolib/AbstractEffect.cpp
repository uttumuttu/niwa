/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "AbstractEffect.h"

namespace niwa {
    namespace demolib {
        void AbstractEffect::setArguments(CheckableArguments const&) {
            // ignored
        }

        void AbstractEffect::update(double secondsElapsed) {
            // ignored
        }

        void AbstractEffect::onNormalKeys(unsigned char key, int modifiers) {
            // ignored
        }

        void AbstractEffect::onSpecialKeys(int key, int modifiers) {
            // ignored
        }
    }
}