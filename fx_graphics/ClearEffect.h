/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef CLEAREFFECT_H
#define CLEAREFFECT_H

#include "niwa/demolib/AbstractEffect.h"
#include "niwa/demolib/Arguments.h"

/**
 * ClearEffect takes one optional argument called "color",
 * whose type is a 4-vector (default value {0,0,0,1}).
 */
class ClearEffect : public niwa::demolib::AbstractEffect {
public:
    explicit ClearEffect(niwa::demolib::CheckableArguments const&);

    void setArguments(niwa::demolib::CheckableArguments const&);

    void render(niwa::demolib::IGraphics const&) const;

private:
    niwa::demolib::Arguments args_;
};

#endif