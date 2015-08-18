/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef CLOTHEFFECT_H
#define CLOTHEFFECT_H

#include "niwa/demolib/AbstractEffect.h"

#include <memory>
#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace dynamics {
        class Cloth;
        
        namespace rendering {
            class ClothRenderer;
        }
    }

    namespace graphics {
        class Occluder;
        class ShadowVolumeRenderer;
    }
}

class ClothEffect : public niwa::demolib::AbstractEffect {
public:
    ClothEffect();

    void render(niwa::demolib::IGraphics const&) const;

    void update(double secondsElapsed);

private: // prevent copying
    ClothEffect(ClothEffect const&);
    ClothEffect& operator = (ClothEffect const&);

private:
    class RenderingCallback;

    void renderObjects() const;

private:
    boost::shared_ptr<niwa::dynamics::Cloth> cloth_;

    boost::shared_ptr<niwa::graphics::Occluder> occluder_;

    std::auto_ptr<niwa::dynamics::rendering::ClothRenderer> renderer_;

    std::auto_ptr<niwa::graphics::ShadowVolumeRenderer> shadowVolume_;

    double timeSeconds_;

    friend class RenderingCallback;
};

#endif