/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "Morphing.h"

#include "niwa/demolib/CheckableArguments.h"
#include "niwa/demolib/IGraphics.h"

#include "niwa/graphics/MarchingCubes.h"

#include "niwa/levelset/objects/Grid.h"
#include "niwa/levelset/objects/GridBuilder.h"

#include "niwa/system/NiwaParallelizer.h"

#include "niwa/autodesk/Importer.h"
#include "niwa/autodesk/Model.h"

#define SSE

#ifdef SSE
#include <xmmintrin.h>
#endif

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "niwa/logging/Logger.h"

using namespace niwa::autodesk;
using namespace niwa::demolib;
using namespace niwa::geom;
using namespace niwa::graphics;
using namespace niwa::levelset::objects;
using namespace niwa::logging;
using namespace niwa::math;
using namespace niwa::system;

namespace {
    static Logger Log(typeid(Morphing));
}

Morphing::Morphing(CheckableArguments const& args) : timeSeconds_(0), args_(args) {
    marchingCubes_ = std::auto_ptr<MarchingCubes>(new MarchingCubes());
    parallelizer_ = boost::shared_ptr<IParallelizer>(NiwaParallelizer::create());

    marchingCubes_->setUseVertexNormals(
        args.get("vertex_normals").asNumber<int>(marchingCubes_->getUseVertexNormals()) != 0);

    int resolution = args.checkInteger("resolution");

    if(resolution <= 1) {
        args.error("resolution must be positive");
    }

    Importer importer;

    std::auto_ptr<Model> sourceModel(
        importer.importModel(args.checkString("source_model")));

    std::auto_ptr<Model> destinationModel(
        importer.importModel(args.checkString("destination_model")));

    if(sourceModel.get() && destinationModel.get()) {
        GridBuilder builder(
            aabb(vec3f(-1,-1,-1), vec3f(1,1,1)),
            vec3i(resolution, resolution, resolution));

        sourceGrid_ = std::auto_ptr<Grid>(builder.build(*sourceModel));

        destinationGrid_ = std::auto_ptr<Grid>(builder.build(*destinationModel));

        GridBuilder workingBuilder(
            sourceGrid_->getBounds(),
            sourceGrid_->getDimensions());

        workingGrid_ = std::auto_ptr<Grid>(workingBuilder.buildEmpty());
    } else {
        args.error("cannot load models");
    }
}

Morphing::~Morphing() {
    // ignored
}

void Morphing::setArguments(niwa::demolib::CheckableArguments const& args) {
    marchingCubes_->setUseVertexNormals(
        args.get("vertex_normals").asNumber<int>(marchingCubes_->getUseVertexNormals()) != 0);
}

void Morphing::render(niwa::demolib::IGraphics const& g) const {
    const double t = timeSeconds_ * 10;

    glViewport(0, 0, g.getWidth(), g.getHeight());

    glClearColor(0.0f,0.0f,0.0f,1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(!workingGrid_.get()) {
        // the effect is in wrong state, skip rendering
        return;
    }

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    gluPerspective(45.0, g.getAspectRatio(), 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTranslated(0,0,-3);

    glRotated(t*2, 1,0,0);
    glRotated(t*3, 0,1,0);
    glRotated(t*5, 0,0,1);

    float alpha = static_cast<float>(1 - sin(t * .1f)) * .5f;

    const vec3i dim = workingGrid_->getDimensions();

    int n = dim.x * dim.y * dim.z;

    float* source = sourceGrid_->getRaw();
    float* destination = destinationGrid_->getRaw();
    float* working = workingGrid_->getRaw();

    int i = 0;

#ifdef SSE
    __m128* source128 = reinterpret_cast<__m128*>(source);
    __m128* destination128 = reinterpret_cast<__m128*>(destination);
    __m128* working128 = reinterpret_cast<__m128*>(working);

    __m128 alpha128 = _mm_set_ps1(alpha);

    for(; i<n; i+=4) {
        *working128 = _mm_add_ps(
            *source128,
            _mm_mul_ps(
              _mm_sub_ps(*destination128, *source128),
              alpha128));

        ++source128;
        ++destination128;
        ++working128;
    }
#endif

    for(; i<n; ++i) {
        working[i] = source[i] + (destination[i] - source[i]) * alpha;
    }

    marchingCubes_->render(*workingGrid_);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void Morphing::update(double secondsElapsed) {
    timeSeconds_ += secondsElapsed;
}

void Morphing::onNormalKeys(unsigned char key, int /*modifiers*/) {
    if(key == ' ') {
        marchingCubes_->setUseOpenGl(
            !marchingCubes_->getUseOpenGl());
    }
}
