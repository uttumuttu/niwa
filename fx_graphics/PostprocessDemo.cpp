/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "PostprocessDemo.h"

#include "niwa/demolib/IGraphics.h"

#include "niwa/graphics/FrameBuffer.h"
#include "niwa/graphics/Program.h"
#include "niwa/graphics/RenderBuffer.h"
#include "niwa/graphics/Spectrum.h"
#include "niwa/graphics/Texture.h"

#include <cmath>

#include <gl/glew.h>

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#define N_SPHERES 8

using namespace niwa::graphics;

#define TEXTURE_WIDTH 256
#define TEXTURE_HEIGHT 256

#define N_JACOBI_ITERATIONS 32

#define REGULARIZATION 3

#define DIMENSIONS std::pair<size_t, size_t>(TEXTURE_WIDTH, TEXTURE_HEIGHT)

namespace {
    static Texture* createTexture(std::pair<size_t, size_t> dimensions) {
        return Texture::createEmpty(
            dimensions, Texture::RGBA8, Texture::LINEAR, Texture::CLAMP);
    }

    static FrameBuffer* createFrameBuffer(
        std::pair<size_t, size_t> dimensions, boost::shared_ptr<Texture> texture,
        bool useDepth) {
        if(useDepth) {
            boost::shared_ptr<RenderBuffer> depthBuffer(RenderBuffer::create(
                RenderBuffer::DEPTH, dimensions));

            return FrameBuffer::create(texture, depthBuffer);
        } else {
            return FrameBuffer::create(texture);
        }
    }

    static void renderScene(std::pair<size_t, size_t> dimensions, double time) {
        int w = dimensions.first;
        int h = dimensions.second;

        double ratio = h == 0 ? 0 : w / static_cast<double>(h);

        glViewport(0, 0, w, h);

        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        gluPerspective(45.0, ratio, 1, 1000);
        glMatrixMode(GL_MODELVIEW);
        {
            glPushMatrix();
            glLoadIdentity();
            glTranslated(0,0,-2-N_SPHERES);
            glRotated(time*250, 0,0,1);

            glColor3f(0,0,0);

            glBegin(GL_QUADS);
            glVertex2f(-1,-3);
            glVertex2f(1,-3);
            glVertex2f(1,3);
            glVertex2f(-1,3);
            glEnd();
            glPopMatrix();

            boost::shared_ptr<GLUquadric> quadric(gluNewQuadric(), gluDeleteQuadric);

            glColor3f(0,0,0);

            for(int i=0; i<N_SPHERES; ++i) {
                glPushMatrix();
                glLoadIdentity();
                {
                    double deg = sin(time) * (i+1) * 100;

                    glTranslated(0,0,-2-i);
                    glRotated(deg,0,0,1);
                    glTranslated(1,0,0);

                    glColor3d(
                        (sin(i*2+1.0)+1)*.5,
                        (cos(i*4+1.0)+1)*.5,
                        (sin(i*3+2.0)+1)*.5);

                    gluSphere(quadric.get(), 0.3f, 32, 32);
                }
                glPopMatrix();
            }
        }
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glDisable(GL_DEPTH_TEST);
    }

    static void renderFullscreenQuad(std::pair<size_t, size_t> dimensions, Spectrum color) {
        glViewport(0, 0, dimensions.first, dimensions.second);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glColor3f(color.r, color.g, color.b);

        glBegin(GL_QUADS);
        glVertex2f(-1,-1);
        glVertex2f(1,-1);
        glVertex2f(1,1);
        glVertex2f(-1,1);
        glEnd();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }

    static void renderFullscreen(std::pair<size_t, size_t> dimensions, Texture const& texture) {
        glViewport(0, 0, dimensions.first, dimensions.second);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glEnable(GL_TEXTURE_2D);
        {
            glColor3f(1,1,1);

            Texture::ScopedBind bind(texture);
            glBegin(GL_QUADS);
            glTexCoord2f(0,0);
            glVertex2f(-1,-1);

            glTexCoord2f(1,0);
            glVertex2f(1,-1);

            glTexCoord2f(1,1);
            glVertex2f(1,1);

            glTexCoord2f(0,1);
            glVertex2f(-1,1);
            glEnd();
        }
        glDisable(GL_TEXTURE_2D);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }
}

PostprocessDemo::PostprocessDemo() 
: time_(0),

differentialProgram_(Program::create(
    "shaders/optical_differential.vert",
    "shaders/optical_differential.frag")),

flowProgram_(Program::create(
    "shaders/horn_schunck.vert",
    "shaders/horn_schunck.frag")),

blurProgram_(Program::create(
    "shaders/motion_blur.vert",
    "shaders/motion_blur.frag")),

newTexture_(createTexture(DIMENSIONS)),
oldTexture_(createTexture(DIMENSIONS)),
newVelocityTexture_(createTexture(DIMENSIONS)),
oldVelocityTexture_(createTexture(DIMENSIONS)),
differentialTexture_(createTexture(DIMENSIONS)),
blurTexture_(createTexture(DIMENSIONS)),

newFrame_(createFrameBuffer(DIMENSIONS, newTexture_, true)),
oldFrame_(createFrameBuffer(DIMENSIONS, oldTexture_, true)),
newVelocityFrame_(createFrameBuffer(DIMENSIONS, newVelocityTexture_, false)),
oldVelocityFrame_(createFrameBuffer(DIMENSIONS, oldVelocityTexture_, false)),
differentialFrame_(createFrameBuffer(DIMENSIONS, differentialTexture_, false)),
blurFrame_(createFrameBuffer(DIMENSIONS, blurTexture_, false)) {
    // ignored
}

void PostprocessDemo::render(niwa::demolib::IGraphics const& g) const {
    // TODO: make sure the new swap thingy works!
    newTexture_.swap(oldTexture_);
    newFrame_.swap(oldFrame_);
    //std::swap<boost::shared_ptr<Texture>>(newTexture_, oldTexture_);
    //std::swap<boost::shared_ptr<FrameBuffer>>(newFrame_, oldFrame_);

    // render new frame
    {
        FrameBuffer::ScopedBind bind(*newFrame_);
        renderScene(DIMENSIONS, time_);
    }

    // compute optical differential
    { 
        FrameBuffer::ScopedBind frameBind(*differentialFrame_);

        Program::ScopedUse use(*differentialProgram_);

        Texture::ScopedBind texBind0(*newTexture_, 0);
        Texture::ScopedBind texBind1(*oldTexture_, 1);

        use.setIntValue("new_frame", 0);
        use.setIntValue("old_frame", 1);

        use.setFloatValue("pixel_width", 1.0f / TEXTURE_WIDTH);
        use.setFloatValue("pixel_height", 1.0f / TEXTURE_HEIGHT);

        renderFullscreenQuad(DIMENSIONS, Spectrum(1,1,1));
    }

    // initialize flow iteration
    {
        FrameBuffer::ScopedBind bind(*newVelocityFrame_);

        Spectrum zeroVelocity = Spectrum(1,1,1) * 0.5f;

        renderFullscreenQuad(DIMENSIONS, zeroVelocity);
    }

    // perform flow iteration
    {
        Program::ScopedUse use(*flowProgram_);

        Texture::ScopedBind texBind0(*differentialTexture_, 0);

        use.setIntValue("optical_differential", 0);

        use.setFloatValue("regularization", REGULARIZATION);

        use.setFloatValue("pixel_width", 1.0f / TEXTURE_WIDTH);
        use.setFloatValue("pixel_height", 1.0f / TEXTURE_HEIGHT);

        for(int i=0; i<N_JACOBI_ITERATIONS; ++i) {
            // TODO: make sure the new swap thing works
            newVelocityTexture_.swap(oldVelocityTexture_);
            newVelocityFrame_.swap(oldVelocityFrame_);
            //std::swap<boost::shared_ptr<Texture>>(newVelocityTexture_, oldVelocityTexture_);
            //std::swap<boost::shared_ptr<FrameBuffer>>(newVelocityFrame_, oldVelocityFrame_);

            FrameBuffer::ScopedBind bind(*newVelocityFrame_);

            Texture::ScopedBind texBind1(*oldVelocityTexture_, 1);

            use.setIntValue("current_velocity", 1);

            renderFullscreenQuad(DIMENSIONS, Spectrum(1,1,1));
        }
    }

    // perform motion blur
    {
        FrameBuffer::ScopedBind bind(*blurFrame_);

        Program::ScopedUse use(*blurProgram_);

        Texture::ScopedBind texBind0(*newTexture_, 0);
        Texture::ScopedBind texBind1(*oldTexture_, 1);
        Texture::ScopedBind texBind2(*newVelocityTexture_, 2);

        use.setIntValue("new_frame", 0);
        use.setIntValue("old_frame", 1);
        use.setIntValue("optical_flow", 2);

        use.setFloatValue("shutter_time", 1);

        renderFullscreenQuad(DIMENSIONS, Spectrum(1,1,1));
    }

    renderFullscreen(g.getDimensions(), *blurTexture_);
}

void PostprocessDemo::update(double secondsElapsed) {
    time_ += secondsElapsed;
}