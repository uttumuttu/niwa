/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "RaytraceBindings.h"

#include "niwa/demolib/CheckableArguments.h"

#include "niwa/raytrace/Camera.h"

#include "niwa/raytrace/objects/Sphere.h"
#include "niwa/raytrace/objects/CornellBoxWalls.h"
#include "niwa/raytrace/objects/Mesh.h"
#include "niwa/raytrace/objects/SquareLight.h"

#include "niwa/autodesk/Importer.h"
#include "niwa/autodesk/Model.h"

#include "niwa/logging/Logger.h"

#include "niwa/geom/aabb.h"

using namespace niwa::autodesk;
using namespace niwa::logging;
using namespace niwa::graphics;
using namespace niwa::math;
using namespace niwa::geom;
using namespace niwa::demolib;
using namespace niwa::raytrace;
using namespace niwa::raytrace::objects;

namespace {
    Logger Log("RaytraceBinding");
}

namespace {
    static Mesh* loadMesh(CheckableArguments const& args) {
        Importer importer;

        aabb bounds(vec3f(-1,-1,-1), vec3f(1,1,1));

        if(args.get("bounds").isMatrix(2,3)) {
            std::vector<float> values = args.get("bounds").asMatrix<float>();

            bounds = aabb(
                vec3f(values[0], values[1], values[2]),
                vec3f(values[3], values[4], values[5]));
        } else {
            Log.debug() << "no legal bounds argument, using default bounds";
        }

        if(args.get("model").isString()) {
            std::string filename = args.get("model").asString();

            std::auto_ptr<Model> model( importer.importModel(filename) );

            if(model.get()) {
                return new Mesh(*model, bounds);
            } else {
                Log.warn() << "cannot load model " << filename;
                return new Mesh();
            }
        } else {
            Log.warn() << "missing model argument";
            return new Mesh();
        }
    }
}

SphereBinding::SphereBinding(CheckableArguments const& args) 
: RaytraceBinding(new Sphere()) {
    std::vector<double> zeros;

    for(int i=0; i<3; ++i) {
        zeros.push_back(0);
    }

    args_.set("material", Argument("diffuse")); // defaults
    args_.set("color", Argument(zeros));
    args_.set("refractive_index", Argument(1.0));

    args_.set(args);
}

void SphereBinding::setArguments(CheckableArguments const& args) {
    args_.set(args);
}

void SphereBinding::propagateArguments(double timeSeconds) {
    object_->setPosition(vec3f(
        args_.getAtTime("position", timeSeconds).asVector<float,3>(
            object_->getPosition().getRaw())));

    object_->setRadius(
        args_.getAtTime("radius", timeSeconds).asNumber<float>(
            object_->getRadius()));

    std::string material = args_.getAtTime("material", timeSeconds).asString();

    if(material == "diffuse") {
        Spectrum color(args_.getAtTime("color", timeSeconds).asVector<float,3>());

        object_->setMaterial(Material::createDiffuse(Spectrum(color)));
    } else if(material == "specular") {
        Spectrum color(args_.getAtTime("color", timeSeconds).asVector<float,3>());

        object_->setMaterial(Material::createSpecular(Spectrum(color)));
    } else if(material == "dielectric") {
        object_->setMaterial(Material::createDielectric(
            args_.getAtTime("refractive_index", timeSeconds).asNumber<float>(1.0f)));
    }
}

CornellBoxWallsBinding::CornellBoxWallsBinding(CheckableArguments const&) 
: RaytraceBinding(new CornellBoxWalls()) {
    // ignored
}

MeshBinding::MeshBinding(CheckableArguments const& args) 
: RaytraceBinding(loadMesh(args)) {
    // ignored
}

SquareLightBinding::SquareLightBinding(CheckableArguments const& args)
: RaytraceBinding(new SquareLight()), args_(args) {
    // ignored
}

void SquareLightBinding::setArguments(CheckableArguments const& args) {
    args_.set(args);
}

void SquareLightBinding::propagateArguments(double timeSeconds) {
    object_->setPosition(vec3f(args_.getAtTime("position", timeSeconds).asVector<float,3>(
        object_->getPosition().getRaw())));

    object_->setBasis1(vec3f(args_.getAtTime("basis_1", timeSeconds).asVector<float,3>(
        object_->getBasis1().getRaw())));

    object_->setBasis2(vec3f(args_.getAtTime("basis_2", timeSeconds).asVector<float,3>(
        object_->getBasis2().getRaw())));

    object_->setPower(Spectrum(args_.getAtTime("power", timeSeconds).asVector<float, 3>(
        object_->getPower().getRaw())));

    object_->setSampleCount(args_.getAtTime("sample_count", timeSeconds).asNumber<size_t>(
        object_->getSampleCount()));
}

CameraBinding::CameraBinding(CheckableArguments const& args)
: RaytraceBinding(new Camera()) {
    std::vector<double> position;
    position.push_back(0);
    position.push_back(0);
    position.push_back(0);

    std::vector<double> target;
    target.push_back(0);
    target.push_back(0);
    target.push_back(-1);

    args_.set("position", Argument(position));
    args_.set("target", Argument(target));

    args_.set(args);
}

void CameraBinding::setArguments(CheckableArguments const& args) {
    args_.set(args);
}

void CameraBinding::propagateArguments(double timeSeconds) {
    vec3f position( args_.getAtTime("position", timeSeconds).asVector<float,3>() );
    vec3f lookat( args_.getAtTime("lookat", timeSeconds).asVector<float,3>() );

    object_->setLookat(position, lookat);
}