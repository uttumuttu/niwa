/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef FULLSCREENSHADER_H
#define FULLSCREENSHADER_H

#include "niwa/demolib/AbstractEffect.h"
#include "niwa/demolib/Arguments.h"

#include <memory>

namespace niwa {
    namespace graphics {
        class Program;
    }
}

/**
 * A full-screen shader has two string-type niwa::demolib arguments:
 * vertex_shader and fragment_shader, which point to
 * the files containing the GLSL shaders.
 *
 * If the full-screen shader is provided more niwa::demolib arguments,
 * they are sent to the shaders. Currently the full-screen shader
 * can send integers, floats, and vectors thereof of dimension one to four.
 * Arrays or matrices can't be sent.
 */
class FullScreenShader : public niwa::demolib::AbstractEffect {
public:
    FullScreenShader(niwa::demolib::CheckableArguments const& args);
    ~FullScreenShader();

    void setArguments(niwa::demolib::CheckableArguments const&);

    void render(niwa::demolib::IGraphics const&) const;

    void update(double secondsElapsed);

private: // prevent copying
    FullScreenShader(FullScreenShader const&);
    FullScreenShader& operator = (FullScreenShader const&);

private:
    mutable std::auto_ptr<niwa::graphics::Program> program_;

    niwa::demolib::Arguments args_;

    double timeSeconds_;
};

#endif