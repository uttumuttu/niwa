/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef RIGIDBODYEFFECT_H
#define RIGIDBODYEFFECT_H

#include "niwa/demolib/AbstractEffect.h"

#include <memory>
#include <boost/shared_ptr.hpp>

class RigidBodyEffect : public niwa::demolib::AbstractEffect {
public:
    RigidBodyEffect();

    void render(niwa::demolib::IGraphics const&) const;

    void update(double secondsElapsed);

private: // prevent copying
    RigidBodyEffect(RigidBodyEffect const&);
    RigidBodyEffect& operator = (RigidBodyEffect const&);

private:
    double timeSeconds_;
};

#endif