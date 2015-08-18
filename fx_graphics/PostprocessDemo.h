/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef POSTPROCESSDEMO_H
#define POSTPROCESSDEMO_H

#include "niwa/demolib/AbstractEffect.h"

namespace niwa {
    namespace graphics {
        class FrameBuffer;
        class Program;
        class Texture;
    }
}

#include <memory>

#include <boost/shared_ptr.hpp>

/**
 * A demonstration of post-processing effects.
 * Takes no parameters.
 */
class PostprocessDemo : public niwa::demolib::AbstractEffect {
public:
    PostprocessDemo();

    void render(niwa::demolib::IGraphics const&) const;

    void update(double secondsElapsed);

private:
    double time_;

    std::auto_ptr<niwa::graphics::Program> differentialProgram_;
    std::auto_ptr<niwa::graphics::Program> flowProgram_;
    std::auto_ptr<niwa::graphics::Program> blurProgram_;

    mutable boost::shared_ptr<niwa::graphics::Texture> newTexture_;
    mutable boost::shared_ptr<niwa::graphics::Texture> oldTexture_;
    mutable boost::shared_ptr<niwa::graphics::Texture> newVelocityTexture_;
    mutable boost::shared_ptr<niwa::graphics::Texture> oldVelocityTexture_;
    boost::shared_ptr<niwa::graphics::Texture> differentialTexture_;
    boost::shared_ptr<niwa::graphics::Texture> blurTexture_;

    mutable boost::shared_ptr<niwa::graphics::FrameBuffer> newFrame_;
    mutable boost::shared_ptr<niwa::graphics::FrameBuffer> oldFrame_;
    mutable boost::shared_ptr<niwa::graphics::FrameBuffer> newVelocityFrame_;
    mutable boost::shared_ptr<niwa::graphics::FrameBuffer> oldVelocityFrame_;
    boost::shared_ptr<niwa::graphics::FrameBuffer> differentialFrame_;
    boost::shared_ptr<niwa::graphics::FrameBuffer> blurFrame_;
};

#endif