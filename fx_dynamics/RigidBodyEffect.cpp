/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "RigidBodyEffect.h"

#include "niwa/demolib/IGraphics.h"

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

RigidBodyEffect::RigidBodyEffect() : timeSeconds_(0) {
    // ignored
}

void RigidBodyEffect::render(niwa::demolib::IGraphics const& g) const {
    glViewport(0, 0, g.getWidth(), g.getHeight());

    glClearColor(0,0,0,0);

    glClear(GL_COLOR_BUFFER_BIT);
}

void RigidBodyEffect::update(double secondsElapsed) {
    timeSeconds_ += secondsElapsed;
}