/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DEMOLIB_ABSTRACTEFFECT_H
#define NIWA_DEMOLIB_ABSTRACTEFFECT_H

#include "IEffect.h"

namespace niwa {
    namespace demolib {
        /**
         * A demo effect with empty implementations
         * for all methods except rendering.
         */
        class AbstractEffect : public IEffect {
        public:
            void setArguments(CheckableArguments const&);

            void update(double secondsElapsed);

            void onNormalKeys(unsigned char key, int modifiers);

            void onSpecialKeys(int key, int modifiers);
        };
    }
}

#endif