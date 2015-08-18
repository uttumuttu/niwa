/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DEMOLIB_IEFFECT_H
#define NIWA_DEMOLIB_IEFFECT_H

#include "IObject.h"

namespace niwa {
    namespace demolib {
        class IGraphics;

        /**
         * A demo effect.
         */
        class IEffect : public IObject {
        public:
            /**
             * @brief Renders the effect.
             *
             * The implementation should strive to clean up
             * its OpenGL state changes.
             */
            virtual void render(IGraphics const&) const = 0;
        
            virtual void update(double secondsElapsed) = 0;

            /**
             * Processes normal keypresses (using GLUT keycodes).
             */
            virtual void onNormalKeys(unsigned char key, int modifiers) = 0;

            /**
             * Processes special keypresses (using GLUT keycodes).
             */
            virtual void onSpecialKeys(int key, int modifiers) = 0;
        };
    }
}

#endif