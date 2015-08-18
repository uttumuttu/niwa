/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "FullScreenShader.h"

#include "niwa/demolib/CheckableArguments.h"
#include "niwa/demolib/IGraphics.h"

#include "niwa/graphics/Program.h"

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

#include "niwa/logging/Logger.h"

namespace {
    static niwa::logging::Logger log(typeid(FullScreenShader));
}

using namespace niwa::graphics;

FullScreenShader::FullScreenShader(niwa::demolib::CheckableArguments const& args) : args_(args) {
    program_ = std::auto_ptr<Program>(Program::create(
        args.checkString("vertex_shader"),
        args.checkString("fragment_shader")));
}

FullScreenShader::~FullScreenShader() {
    // ignored
}

void FullScreenShader::render(niwa::demolib::IGraphics const& g) const {
    glViewport(0, 0, g.getWidth(), g.getHeight());

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    if(program_.get()) {
        Program::ScopedUse use(*program_);

        use.setFloatValue(
            "time_seconds",
            static_cast<float>(timeSeconds_));

        for(size_t i=0; i<args_.size(); ++i) {
            std::string name = args_.nameAt(i);

            // Since we have no way of knowing
            // the type of the uniform variables,
            // we'll send them as both integer
            // and floating-point types.

            if(name != "vertex_shader" && name != "fragment_shader") {
                niwa::demolib::Argument arg = args_.getAtTime(
                    name, timeSeconds_);

                switch(arg.type()) {
                case niwa::demolib::Argument::NUMBER:
                    use.setIntValue(name, arg.asNumber<int>());
                    use.setFloatValue(name, arg.asNumber<float>());
                    break;
                case niwa::demolib::Argument::STRING:
                    // ignored
                    break;
                case niwa::demolib::Argument::VECTOR:
                    use.setIntValues(name, arg.asVector<int>());
                    use.setFloatValues(name, arg.asVector<float>());
                    break;
                case niwa::demolib::Argument::MATRIX:
                    use.setMatrix(
                        name, arg.matrixDimensions(), 
                        true, arg.asMatrix<float>());
                    break;
                default:
                    break;
                }
            }
        }

        glBegin(GL_QUADS);
        glVertex2i(-1,-1);
        glVertex2i(-1, 1);
        glVertex2i( 1, 1);
        glVertex2i( 1,-1);
        glEnd();
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void FullScreenShader::update(double secondsElapsed) {
    timeSeconds_ += secondsElapsed;
}

void FullScreenShader::setArguments(niwa::demolib::CheckableArguments const& args) {
    args_.set(args);
}