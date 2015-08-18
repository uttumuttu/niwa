/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef HARDWARECAUSTICS_H
#define HARDWARECAUSTICS_H

#include "niwa/demolib/AbstractEffect.h"

#include "niwa/math/vec3f.h"

#include "niwa/graphics/Spectrum.h"

#include <vector>

namespace niwa {
    namespace graphics {
        class FrameBuffer;
        class Texture;
    }
}

#include <boost/shared_ptr.hpp>
#include <memory>

/**
 * This is an effect that demonstrates OpenGL
 * frame buffers (render to texture) to render
 * reflections of caustics.
 *
 * The effect takes a texture file parameter
 * (floor_texture) at creation-time.
 */
class HardwareCaustics : public niwa::demolib::AbstractEffect {
public:
    explicit HardwareCaustics(niwa::demolib::CheckableArguments const& args);

    void render(niwa::demolib::IGraphics const&) const;

    void update(double secondsElapsed);

private:
    void renderScene(bool renderRing, niwa::math::vec3f const& eyePosition) const;

private:
    double time_;

    std::auto_ptr<niwa::graphics::Texture> floorTexture_;

    boost::shared_ptr<niwa::graphics::Texture> texture_;

    boost::shared_ptr<niwa::graphics::FrameBuffer> frameBuffer_;
};

#endif