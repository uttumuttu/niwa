/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "ClothEffect.h"

#include "niwa/demolib/IGraphics.h"

#include "niwa/dynamics/Cloth.h"
#include "niwa/dynamics/ClothBuilder.h"
#include "niwa/dynamics/ClothCollider.h"
#include "niwa/dynamics/rendering/ClothRenderer.h"

#include "niwa/graphics/Occluder.h"
#include "niwa/graphics/ShadowVolumeRenderer.h"

#include "niwa/levelset/ILevelSet.h"

#include "niwa/system/NiwaParallelizer.h"

#include "niwa/math/vec2i.h"

using namespace niwa::demolib;
using namespace niwa::dynamics;
using namespace niwa::dynamics::rendering;
using namespace niwa::graphics;
using namespace niwa::levelset;
using namespace niwa::math;
using namespace niwa::system;

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#define FLOOR_POSITION -1.5f

class ClothEffect::RenderingCallback : public ShadowVolumeRenderer::ICallback {
public:
    RenderingCallback(ClothEffect* effect) : effect_(effect) {
        // ignored
    }

    void disableLights() const {
        glDisable(GL_LIGHT0);
    }

    int getLightCount() const {
        return 2;
    }

    void enableLight(int lightIndex) const {
        glEnable(GL_LIGHT0);

        float position[4] = {0,0,0,1};

        float diffuse[4] = {0.5f,0.5f,0.5f,1};

        *reinterpret_cast<vec3f*>(&position) = getLightPosition(lightIndex);

        glLightfv(GL_LIGHT0, GL_POSITION, position);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    }

    vec3f getLightPosition(int lightIndex) const {
        return vec3f(-4.0f * lightIndex, 4.0f, -4.0f * (1-lightIndex));
    }

    void renderObjects() const {
        effect_->renderObjects();
    }

    float const* getAmbientLight() const {
        static const float ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };

        return ambient;
    }

private:
    ClothEffect* effect_;
};

namespace {
    class Floor : public ILevelSet {
    public:
        bool isDistanceField() const {
            return true;
        }

        float __fastcall value(vec3f const& position) const {
            return position.y - FLOOR_POSITION;
        }

        vec3f __fastcall gradient(vec3f const& /*position*/) const {
            return vec3f(0,1,0);
        }
    };
}

namespace {
    Occluder* buildOccluder(Cloth const& cloth) {
        OccluderBuilder builder;

        std::vector<ClothParticle> const& particles = cloth.getParticles();

        std::vector<Cloth::Triangle> const& triangles = cloth.getTriangles();

        for(size_t i=0; i<particles.size(); ++i) {
            builder.addVertex(particles[i].getPosition());
        }

        for(size_t i=0; i<triangles.size(); ++i) {
            int indices[3];

            for(int j=0; j<3; ++j) {
                indices[j] = triangles[i].indices[j];
            }

            builder.addTriangle(indices);
        }

        return builder.build();
    }

    void updateOccluder(Occluder& occluder, Cloth const& cloth) {
        std::vector<vec3f>& vertices = occluder.getVertices();

        std::vector<ClothParticle> const& particles = cloth.getParticles();

        for(size_t i=0; i<particles.size(); ++i) {
            vertices[i] = particles[i].getPosition();
        }
    }
}

ClothEffect::ClothEffect() : timeSeconds_(0) {
    const float nonProjectiveDamping = 0.00002f;
    /*const float projectiveDamping = 0.01f;*/

    ClothBuilder builder(
        2.0f, // mass
        1.0f, 0.02f, nonProjectiveDamping, // force parameters
        0.002f); // thickness

    vec3f sides[2];
    sides[0] = vec3f(2,0,0);
    sides[1] = vec3f(0,0,2);

    vec2i dimensions(64, 64);

    bool constraints[2][2] = { true, true, true, false };

    cloth_ = boost::shared_ptr<Cloth>(
        builder.buildRectangular(
            vec3f(-1,0,-1), sides, dimensions,
            constraints));

    boost::shared_ptr<ILevelSet> floor(new Floor());

    boost::shared_ptr<ClothCollider> collider(
        new ClothCollider(floor, 0.01f));

    cloth_->setCollider(collider);

    //cloth_->setUseImplicitMethod(true);

    //cloth_->setUseStrainLimiting(true);

    boost::shared_ptr<IParallelizer> parallelizer(NiwaParallelizer::create());

    cloth_->setParallelizer(parallelizer);

    renderer_ = std::auto_ptr<ClothRenderer>(
        new ClothRenderer(cloth_));

    boost::shared_ptr<ShadowVolumeRenderer::ICallback> callback(
        new RenderingCallback(this));

    shadowVolume_ = std::auto_ptr<ShadowVolumeRenderer>(
        new ShadowVolumeRenderer(callback));

    occluder_ = boost::shared_ptr<Occluder>(
        buildOccluder(*cloth_));

    shadowVolume_->addOccluder(occluder_);
}

void ClothEffect::render(IGraphics const& g) const {
    glViewport(0, 0, g.getWidth(), g.getHeight());

    glClearColor(1,1,1,1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(
        45.0, g.getAspectRatio(), 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(0,0,-4);

    glRotatef(10,1,0,0);

    glRotated(timeSeconds_ * 10,0,1,0);

    updateOccluder(*occluder_, *cloth_);

    renderer_->precomputeFrame();

    shadowVolume_->renderFrame();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void ClothEffect::renderObjects() const {
    float glTrue[] = { GL_TRUE };
    float glFalse[] = { GL_FALSE };

    float frontDiffuse[] = { 1.0f, 0.7f, 0.3f, 1.0f };
    float backDiffuse[]  = { 0.7f, 0.3f, 1.0f, 1.0f };
    float floorDiffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, frontDiffuse);
    glMaterialfv(GL_BACK,  GL_DIFFUSE, backDiffuse);

    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, glTrue);

    glDisable(GL_CULL_FACE);

    renderer_->render();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, floorDiffuse);

    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, glFalse);

    glNormal3f(0,1,0);

    for(int y=-4; y<4; ++y) {
        glBegin(GL_TRIANGLE_STRIP);
        for(int x=-4; x<=4; ++x) {
            glVertex3d(x,FLOOR_POSITION,y);
            glVertex3d(x,FLOOR_POSITION,y+1);
        }
        glEnd();
    }
}

void ClothEffect::update(double secondsElapsed) {
    for(int i=0; i<8; ++i) {
        cloth_->simulate(1.0f / 800);
    }

    timeSeconds_ += secondsElapsed;
}