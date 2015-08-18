/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef RAYTRACEEFFECT_H
#define RAYTRACEEFFECT_H

#include "niwa/demolib/AbstractEffect.h"

#include <boost/shared_ptr.hpp>

#include <memory>
#include <vector>

namespace niwa {
    namespace demolib {
        class LuaRef;
    }

    namespace raytrace {
        class SimpleRenderer;
        class Camera;
        class IToneMapper;

        namespace objects {
            class CompositeTraceable;
            class CompositeLight;
        }
    }
}

/**
 * TODO: document arguments.
 */
class RaytraceEffect : public niwa::demolib::AbstractEffect {
public:
    RaytraceEffect(niwa::demolib::CheckableArguments const& args);
    ~RaytraceEffect();

    void render(niwa::demolib::IGraphics const&) const;

    void update(double secondsElapsed);

    void onNormalKeys(unsigned char key, int modifiers);

private: // prevent copying
    RaytraceEffect(RaytraceEffect const&);
    RaytraceEffect& operator = (RaytraceEffect const&);

private:
    std::auto_ptr<niwa::raytrace::SimpleRenderer> renderer_;

    boost::shared_ptr<niwa::raytrace::Camera> camera_;

    boost::shared_ptr<niwa::raytrace::objects::CompositeTraceable> compositeObject_;

    boost::shared_ptr<niwa::raytrace::objects::CompositeLight> compositeLight_;

    boost::shared_ptr<niwa::raytrace::IToneMapper> toneMapper_;

    std::vector<boost::shared_ptr<niwa::demolib::LuaRef>> objectRefs_;
    std::vector<boost::shared_ptr<niwa::demolib::LuaRef>> lightRefs_;
    boost::shared_ptr<niwa::demolib::LuaRef> cameraRef_;

    double timeSeconds_;
};

#endif