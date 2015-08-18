/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "RaytraceEffect.h"

#include "RaytraceBindings.h"

#include "niwa/demolib/Arguments.h"
#include "niwa/demolib/CheckableArguments.h"
#include "niwa/demolib/IGraphics.h"
#include "niwa/demolib/LuaRef.h"

#include "niwa/raytrace/Camera.h"
#include "niwa/raytrace/ExponentialToner.h"
#include "niwa/raytrace/SimpleRenderer.h"

#include "niwa/raytrace/objects/CompositeLight.h"
#include "niwa/raytrace/objects/CompositeTraceable.h"

#include "niwa/photonmap/IPhotonMap.h"
#include "niwa/photonmap/PhotonHash.h"
#include "niwa/photonmap/HilbertPhotonHash.h"

#pragma warning(disable:4505)
#include <glut.h>

using namespace niwa;

using namespace niwa::graphics;
using namespace niwa::raytrace::objects;
using namespace niwa::raytrace;
using namespace niwa::math;
using namespace niwa::photonmap;

using boost::shared_ptr;

#define DEFAULT_WINDOW_WIDTH 160
#define DEFAULT_WINDOW_HEIGHT 120

#define DEFAULT_PHOTON_COUNT 0
#define DEFAULT_PHOTON_RADIUS 0.3f
#define DEFAULT_PHOTON_NEIGHBOR_COUNT 10

#define PHOTON_CAPACITY 500000

RaytraceEffect::RaytraceEffect(niwa::demolib::CheckableArguments const& args) 
: timeSeconds_(0) {
    std::vector<shared_ptr<ITraceable>> objects;
    std::vector<shared_ptr<ILight>> lights;

    objectRefs_ = args.checkObjectList("objects");

    for(size_t i=0; i<objectRefs_.size(); ++i) {
        assert(objectRefs_[i]->isValid());

        RaytraceBinding<ITraceable>* binding =
            reinterpret_cast<RaytraceBinding<ITraceable>*>(
                objectRefs_[i]->getObject());

        objects.push_back(binding->getObject());
    }

    lightRefs_ = args.checkObjectList("lights");

    for(size_t i=0; i<lightRefs_.size(); ++i) {
        assert(lightRefs_[i]->isValid());

        RaytraceBinding<ILight>* binding =
            reinterpret_cast<RaytraceBinding<ILight>*>(
                lightRefs_[i]->getObject());

        objects.push_back(binding->getObject()); // lights are also traceable

        lights.push_back(binding->getObject());
    }

    cameraRef_ = args.checkObject("camera");

    assert(cameraRef_->isValid());

    RaytraceBinding<Camera>* binding =
        reinterpret_cast<RaytraceBinding<Camera>*>(
            cameraRef_->getObject());

    camera_ = binding->getObject();

    compositeObject_ = shared_ptr<CompositeTraceable>(new CompositeTraceable());
    compositeObject_->setObjects(objects);

    compositeLight_ = shared_ptr<CompositeLight>(new CompositeLight());
    compositeLight_->setLights(lights);

    toneMapper_ = shared_ptr<IToneMapper>(new ExponentialToner());

    std::pair<size_t, size_t> windowSize(
        DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

    if(args.get("window_size").isVector(2)) {
        std::vector<size_t> values = args.get("window_size").asVector<size_t>();

        windowSize.first = values[0];
        windowSize.second = values[1];
    }

    float aspectRatio = windowSize.first / static_cast<float>(windowSize.second);

    std::string queryType = args.get("photon_query_type").asString();

    int photonCount = args.get("photon_count").asNumber<int>(DEFAULT_PHOTON_COUNT);

    shared_ptr<IPhotonMap> photonMap;

    if(queryType == "range_query") {
        float radius = args.get("photon_query_radius").asNumber<float>(DEFAULT_PHOTON_RADIUS);

        photonMap = shared_ptr<IPhotonMap>(new PhotonHash(PHOTON_CAPACITY, radius));
    } else if(queryType == "neighbor_query") {
        int neighbor_count = args.get("photon_query_neighbor_count").asNumber<int>(DEFAULT_PHOTON_NEIGHBOR_COUNT);

        photonMap = shared_ptr<IPhotonMap>(new HilbertPhotonHash(PHOTON_CAPACITY, neighbor_count));
    } else if(!queryType.empty()) {
        args.error("unsupported photon query type '%s'", queryType.c_str());
    }

    camera_->setBackplaneDimensions(vec2f(aspectRatio, 1.0f));

    renderer_ = std::auto_ptr<SimpleRenderer>(
        new SimpleRenderer(windowSize, photonCount, photonMap));
    renderer_->setCamera(camera_);
    renderer_->setScene(compositeObject_);
    renderer_->setLight(compositeLight_);
    renderer_->setToneMapper(toneMapper_);
}

RaytraceEffect::~RaytraceEffect() {
    // not required
}

namespace {
    static inline double square(double x) {
        return x * x;
    }
}

void RaytraceEffect::render(niwa::demolib::IGraphics const& g) const {
    glViewport(0, 0, g.getWidth(), g.getHeight());

    for(size_t i=0; i<objectRefs_.size(); ++i) {
        assert(objectRefs_[i]->isValid());

        RaytraceBinding<ITraceable>* binding =
            reinterpret_cast<RaytraceBinding<ITraceable>*>(
                objectRefs_[i]->getObject());

        binding->propagateArguments(timeSeconds_);
    }

    for(size_t i=0; i<lightRefs_.size(); ++i) {
        assert(lightRefs_[i]->isValid());

        RaytraceBinding<ILight>* binding =
            reinterpret_cast<RaytraceBinding<ILight>*>(
                lightRefs_[i]->getObject());

        binding->propagateArguments(timeSeconds_);
    }

    {
        assert(cameraRef_->isValid());

        RaytraceBinding<Camera>* binding =
            reinterpret_cast<RaytraceBinding<Camera>*>(
                cameraRef_->getObject());

        binding->propagateArguments(timeSeconds_);
    }

    if(!renderer_->render()) {
        // ignored
    }
}

void RaytraceEffect::update(double secondsElapsed) {
    timeSeconds_ += secondsElapsed;
}

void RaytraceEffect::onNormalKeys(unsigned char key, int /*modifiers*/) {
    if(key == ' ') {
        renderer_->setUseOpenGl(
            !renderer_->getUseOpenGl());
    } else if(key == '1') {
        renderer_->setUseMultithreading(false);
    } else if(key == '2') {
        renderer_->setUseMultithreading(true);
    }
}
