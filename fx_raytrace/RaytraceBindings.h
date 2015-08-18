/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef RAYTRACEBINDINGS_H
#define RAYTRACEBINDINGS_H

#include "niwa/demolib/IObject.h"

#include "niwa/demolib/Arguments.h"

#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace raytrace {
        class ILight;
        class ITraceable;
        class Camera;

        namespace objects {
            class Sphere;
            class CornellBoxWalls;
            class Mesh;
            class SquareLight;
        }
    }
}

/**
 * A binding from ray tracing objects to demolib objects.
 *
 * @param T Must be singly inherited. Multiple inheritance
 *          entails big problem when upcasting generic pointers.
 */
template <typename T>
class RaytraceBinding : public niwa::demolib::IObject {
public:
    explicit RaytraceBinding(T* ptr) : object_(ptr) {
        // ignored
    }

    ~RaytraceBinding() {
        // ignored
    }

    boost::shared_ptr<T> getObject() const {
        return object_;
    }

    void setArguments(niwa::demolib::CheckableArguments const&) {
        // ignored
    }

    /**
     * Propagates the argument values at given time
     * to the underlying raytracing object.
     */
    virtual void propagateArguments(double /*timeSeconds*/) {
        // ignored
    }

protected:
    const boost::shared_ptr<T> object_;
};

class SphereBinding : public RaytraceBinding<niwa::raytrace::objects::Sphere> {
public:
    explicit SphereBinding(niwa::demolib::CheckableArguments const&);

    void setArguments(niwa::demolib::CheckableArguments const&);

    void propagateArguments(double timeSeconds);

private:
    niwa::demolib::Arguments args_;
};

class CornellBoxWallsBinding : public RaytraceBinding<niwa::raytrace::objects::CornellBoxWalls> {
public:
    explicit CornellBoxWallsBinding(niwa::demolib::CheckableArguments const&);
};

class MeshBinding : public RaytraceBinding<niwa::raytrace::objects::Mesh> {
public:
    explicit MeshBinding(niwa::demolib::CheckableArguments const&);
};

class SquareLightBinding : public RaytraceBinding<niwa::raytrace::objects::SquareLight> {
public:
    explicit SquareLightBinding(niwa::demolib::CheckableArguments const&);

    void setArguments(niwa::demolib::CheckableArguments const&);

    void propagateArguments(double timeSeconds);

private:
    niwa::demolib::Arguments args_;
};

class CameraBinding : public RaytraceBinding<niwa::raytrace::Camera> {
public:
    explicit CameraBinding(niwa::demolib::CheckableArguments const&);

    void setArguments(niwa::demolib::CheckableArguments const&);

    void propagateArguments(double timeSeconds);

private:
    niwa::demolib::Arguments args_;
};

#endif