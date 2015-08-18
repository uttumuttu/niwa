/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "HardwareCaustics.h"

#include "niwa/math/Constants.h"
#include "niwa/math/vec2f.h"
#include "niwa/math/vec3f.h"

#include <gl/glew.h>

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include <cmath>
#include <memory>

#include "niwa/graphics/Image.h"
#include "niwa/graphics/FrameBuffer.h"
#include "niwa/graphics/RenderBuffer.h"
#include "niwa/graphics/Texture.h"

#include "niwa/logging/Logger.h"

#include "niwa/demolib/Arguments.h"
#include "niwa/demolib/CheckableArguments.h"
#include "niwa/demolib/IGraphics.h"

#define TEXTURE_WIDTH 512
#define TEXTURE_HEIGHT 512

using niwa::math::constants::PI_F;
using niwa::math::constants::PI_D;

using niwa::demolib::Arguments;
using niwa::demolib::CheckableArguments;

namespace {
    static niwa::logging::Logger Log(typeid(HardwareCaustics));
}

using niwa::graphics::FrameBuffer;
using niwa::graphics::Image;
using niwa::graphics::RenderBuffer;
using niwa::graphics::Spectrum;
using niwa::graphics::Texture;
using niwa::math::vec2f;
using niwa::math::vec3f;

namespace {
    /**
     * A non-physical constant for adjusting the brightness
     * of the caustics.
     */
    static const float CAUSTIC_BRIGHTNESS = 4.0f;

    static const Spectrum RING_INNER_REFLECTANCE(1.0f, 0.9f, 0.8f);
    static const Spectrum RING_OUTER_REFLECTANCE(1.0f, 0.8f, 0.4f);

    static const float REFLECTION_MAP_DISTANCE = 2.0f;

    static const float SHADOW_STRENGTH = 0.5f;

    static const float RING_INNER_RADIUS = 1.0f;
    static const float RING_OUTER_RADIUS = 1.05f;

    static const float RING_HEIGHT = 0.4f;

    static const int RING_TESSELATION_HEIGHT = 8;
    static const int RING_TESSELATION_CIRCUMFERENCE = 32;

    static const float LIGHT_HEIGHT = 2.0f;
    static const float LIGHT_RADIUS = 2.0f;
    static const float LIGHT_JITTER = 0.15f;
    static const float LIGHT_POWER = 200.0f;

    static const float FLOOR_SCALE = 0.5f;
    static const float FLOOR_TEXTURE_SCALE = 0.5f;

    static vec3f openGlEyePosition() {
        float m[16];

        glGetFloatv(GL_MODELVIEW_MATRIX, m);

        float t[3] = {m[12], m[13], m[14]};
        float u[3];

        for(int i=0; i<3; ++i) {
            u[i] = 0;
            for(int j=0; j<3; ++j) {
                u[i] += m[4*i+j] * t[j];
            }
        }
        return vec3f(u);
    }

    static vec3f ringPosition(int x, int y, float r) {
        float rad = static_cast<float>(x) * 2 * PI_F / RING_TESSELATION_CIRCUMFERENCE;

        return vec3f(
            cos(rad) * r, 
            y / static_cast<float>(RING_TESSELATION_HEIGHT), 
            sin(rad) * r);
    }

    static void renderRing(vec3f const& eyePosition) {
        int X = RING_TESSELATION_CIRCUMFERENCE; // shorthands
        int Y = RING_TESSELATION_HEIGHT;

        glEnable(GL_TEXTURE_2D);

        for(int i=0; i<2; ++i) {
            float r = i == 0 ? RING_INNER_RADIUS : RING_OUTER_RADIUS;

            glColor3fv(i==0 
                ? RING_INNER_REFLECTANCE.getRaw() 
                : RING_OUTER_REFLECTANCE.getRaw());

            for(int y=0; y<Y; y++) {
                glBegin(GL_TRIANGLE_STRIP);

                for(int x=0; x<=X; x++) {
                    float xx = cos(x/(float)X*PI_F*2);
                    float zz = sin(x/(float)X*PI_F*2);

                    float y0 = y/(float)Y * RING_HEIGHT;
                    float y1 = (y+1)/(float)Y * RING_HEIGHT;

                    vec3f d0 = vec3f(xx,y0,zz) - eyePosition; d0.normalize();
                    vec3f d1 = vec3f(xx,y1,zz) - eyePosition; d1.normalize();

                    vec3f n(xx,0,zz);

                    d0 -= n*(vec3f::dot(n, d0)*2);
                    d1 -= n*(vec3f::dot(n, d1)*2);

                    vec3f p0 = vec3f(xx*r,y0,zz*r);
                    vec3f p1 = vec3f(xx*r,y1,zz*r);

                    float t0 = -(p0.y/d0.y);
                    float t1 = -(p1.y/d1.y);

                    vec2f T0( p0.x + d0.x*t0, p0.z + d0.z*t0 );
                    vec2f T1( p1.x + d1.x*t1, p1.z + d1.z*t1 );

                    T0 /= REFLECTION_MAP_DISTANCE;
                    T1 /= REFLECTION_MAP_DISTANCE;

                    // Transform from [-1,1] to [0,1].
                    T0 = (T0+vec2f(1,1))*0.5f;
                    T1 = (T1+vec2f(1,1))*0.5f;

                    glTexCoord2f(T0.x, T0.y);
                    glVertex3fv(p0.getRaw());

                    glTexCoord2f(T1.x, T1.y);
                    glVertex3fv(p1.getRaw());
                }

                glEnd();
            }
        }

        glDisable(GL_TEXTURE_2D);

        glBegin(GL_TRIANGLE_STRIP);

        glColor3f(0,0,0);

        for( int x=0; x<=X; x++ ) {
            float rad = x * 2 * PI_F / X;

            glVertex3f(
                cos(rad) * RING_INNER_RADIUS, RING_HEIGHT, sin(rad) * RING_INNER_RADIUS);
            glVertex3f(
                cos(rad) * RING_OUTER_RADIUS, RING_HEIGHT, sin(rad) * RING_OUTER_RADIUS);
        }

        glEnd();
    }

    static void renderFloor(vec3f const& lightPosition) {
        for(int y=-16; y<16; ++y) {
            glBegin(GL_TRIANGLE_STRIP);

            for(int x=-16; x<=16; ++x) {
                for(int dy=0; dy<=1; ++dy) {
                    vec3f position = vec3f(
                        static_cast<float>(x), 0, 
                        static_cast<float>(y+dy)) * FLOOR_SCALE;

                    vec3f direction = position - lightPosition; 

                    float r2 = vec3f::dot(direction, direction);

                    direction.normalize();

                    float B0 = (-direction.y) / (4 * PI_F * r2) * LIGHT_POWER;

                    glColor4f(B0, B0, B0, 1);
                    glTexCoord2f(x * FLOOR_TEXTURE_SCALE, (y+dy) * FLOOR_TEXTURE_SCALE);
                    glVertex3fv(position.getRaw());
                }
            }

            glEnd();
        }
    }

    static vec3f jitteredLightPosition(vec3f const& lightPosition, int x, int y) {
        return lightPosition + vec3f(
            static_cast<float>(x), 0, static_cast<float>(y)) * LIGHT_JITTER;
    }

    static void causticPoint(
            vec3f const& position, vec3f const& normal, vec3f const& lightPosition,
            vec3f& projection, float& irradiosity) {
        vec3f direction = position - lightPosition;

        float r2 = direction.squareLength();

        direction.normalize();

        irradiosity = vec3f::dot(normal, direction) / (4 * r2 * PI_F) * LIGHT_POWER;

        irradiosity *= -direction.y;

        direction -= normal*(vec3f::dot(normal, direction)*2);

        float t = -position.y / direction.y;

        projection = position + direction * t;
    }

    static void renderCaustic(vec3f const& lightPosition) {
        int X = RING_TESSELATION_CIRCUMFERENCE; // shorthand
        int Y = RING_TESSELATION_HEIGHT;

        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_ALPHA, GL_ONE);

        for( int dy=-1; dy<=1; ++dy ) {
            for( int dx=-1; dx<=1; ++dx ) {
                vec3f lpos2(jitteredLightPosition(lightPosition, dx, dy));

                for(int y=0; y<Y; ++y) {
                    glBegin(GL_TRIANGLE_STRIP);

                    vec3f projection;

                    for(int x=0; x<=X; ++x) {
                        float xx = cos(x/(float)X*PI_F*2);
                        float zz = sin(x/(float)X*PI_F*2);

                        for(int dy=0; dy<=1; ++dy) {
                            float y0 = (y+dy)/(float)Y * RING_HEIGHT;

                            float irradiosity;

                            causticPoint(
                                vec3f(xx * RING_OUTER_RADIUS, y0, zz * RING_OUTER_RADIUS),
                                vec3f(xx, 0,zz), lpos2, projection, irradiosity);

                            Spectrum B = 
                                RING_INNER_REFLECTANCE * CAUSTIC_BRIGHTNESS * irradiosity / 9.0f;

                            glColor4f(B.r, B.g, B.b, 0);
                            glVertex3fv(projection.getRaw());
                        }
                    }

                    glEnd();
                }
            }
        }

        glDisable(GL_BLEND);

        glEnable(GL_DEPTH_TEST);
    }

    static void renderShadow(vec3f const& lightPosition) {
        int X = RING_TESSELATION_CIRCUMFERENCE; // shorthand

        glColor4f(0,0,0, pow(1-SHADOW_STRENGTH, 1/9.0f));

        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

        for( int dy=-1; dy<=1; ++dy ) {
            for( int dx=-1; dx<=1; ++dx ) {
                vec3f lpos2(jitteredLightPosition(lightPosition, dx, dy));

                glBegin(GL_TRIANGLE_STRIP);

                for( int x=0; x<=X; x++ ) {
                    vec3f p0(ringPosition(x,RING_TESSELATION_HEIGHT,RING_OUTER_RADIUS));

                    vec3f p1(ringPosition(x,0,RING_OUTER_RADIUS));

                    vec3f direction = p0 - lpos2;

                    float t = -p0.y / direction.y;

                    p0 += direction * t;

                    glVertex3fv(p0.getRaw());
                    glVertex3fv(p1.getRaw());
                }

                glEnd();
            }
        }

        glDisable(GL_BLEND);

        glEnable(GL_DEPTH_TEST);
    }
}

void HardwareCaustics::renderScene(bool renderRing_, vec3f const& eyePosition) const {
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    float time = static_cast<float>(time_);

    vec3f lightPosition(
        cos(time * 1.57812637461f) * LIGHT_RADIUS,
        LIGHT_HEIGHT,
        sin(time * 2.187682763985f) * LIGHT_RADIUS);

    glEnable(GL_TEXTURE_2D);
    {
        Texture::ScopedBind bind(*floorTexture_);
        renderFloor(lightPosition);
    }
    glDisable(GL_TEXTURE_2D);

    renderCaustic(lightPosition);
    renderShadow(lightPosition);

    if(renderRing_) {
        Texture::ScopedBind bind(*texture_);

        renderRing(eyePosition);
    }

    glDisable(GL_DEPTH_TEST);
}

HardwareCaustics::HardwareCaustics(CheckableArguments const& args) : time_(0) {
    std::string filename = args.checkString("floor_texture");

    boost::shared_ptr<Image> image(Image::create(filename, Image::RGBA));

    if(image.get()) {
        size_t nPixels = image->getWidth() * image->getHeight();

        boost::shared_array<unsigned char> data(image->getData());

        for(size_t i=0; i<nPixels; ++i) {
            int sum = 0;
            for(size_t j=0; j<3; ++j) {
                sum += data[4*i+j];
            }
            data[4*i+3] = static_cast<unsigned char>(sum / 3);
        }

        floorTexture_ = std::auto_ptr<Texture>(Texture::fromImage(image,
            Texture::RGBA8,
            Texture::MIPMAP, Texture::REPEAT));
    } else {
        Log.warn() << "cannot load " << filename;
    }

    std::pair<size_t, size_t> dimensions(TEXTURE_WIDTH, TEXTURE_HEIGHT);

    /// @xxx: For some reason, only LINEAR seems to work
    //        with animated textures (at least without manual steps).

    texture_ = boost::shared_ptr<Texture>(Texture::createEmpty(
        dimensions, Texture::RGBA8,
        Texture::LINEAR, Texture::CLAMP));

    boost::shared_ptr<RenderBuffer> depthBuffer(
        RenderBuffer::create(RenderBuffer::DEPTH, dimensions));

    frameBuffer_ = boost::shared_ptr<FrameBuffer>(
        FrameBuffer::create(texture_, depthBuffer));

    if(frameBuffer_->isComplete()) {
        Log.info() << "framebuffer is complete";
    } else {
        Log.warn() << "framebuffer is not complete";
    }
}

void HardwareCaustics::render(niwa::demolib::IGraphics const& g) const {
    {
        FrameBuffer::ScopedBind bind(*frameBuffer_);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();

        glLoadIdentity();
        glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
        gluPerspective(90, TEXTURE_WIDTH / (float)TEXTURE_HEIGHT,1,1000);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        {
            glTranslatef(0,0,-REFLECTION_MAP_DISTANCE);
            glRotatef(-90,1,0,0);

            renderScene(false, vec3f(0,0,0));
        }
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }

    glViewport(0, 0, g.getWidth(), g.getHeight());

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    gluPerspective(45.0, g.getAspectRatio(), 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(0,0,-4);

    glRotated(30 + 20*cos(time_),1,0,0);

    glRotated(time_*10, 0, 1, 0);

    vec3f eyePosition(openGlEyePosition());

    renderScene(true, eyePosition);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void HardwareCaustics::update(double secondsElapsed) {
    time_ += secondsElapsed;
}