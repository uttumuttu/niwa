/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "ClearEffect.h"

#include "niwa/demolib/CheckableArguments.h"

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

using niwa::demolib::Argument;

ClearEffect::ClearEffect(niwa::demolib::CheckableArguments const& args) : args_(args) {
    // ignored
}

void ClearEffect::setArguments(niwa::demolib::CheckableArguments const& args) {
    args_.set(args);
}

void ClearEffect::render(niwa::demolib::IGraphics const&) const {
    float defaultColor[4] = {0,0,0,1};

    Argument::Array<float, 4> color = args_.get("color").asVector<float,4>(defaultColor);

    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}
