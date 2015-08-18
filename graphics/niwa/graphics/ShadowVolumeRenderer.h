/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_GRAPHICS_SHADOWVOLUMERENDERER_H
#define NIWA_GRAPHICS_SHADOWVOLUMERENDERER_H

#include <boost/shared_ptr.hpp>

#include "niwa/math/vec3f.h"

#include <vector>

namespace niwa {
    namespace graphics {
        class Occluder;

        /**
         * Callback-based shadow-volume rendering framework
         * after Everitt & Kilgard's 2002 paper "Practical and Robust
         * Stenciled Shadow Volumes for Hardwarde-Accelerated
         * Rendering".
         */
        class ShadowVolumeRenderer {
        public:
            class ICallback {
            public:
                virtual ~ICallback();

                virtual void disableLights() const = 0;

                virtual float const* getAmbientLight() const = 0;

                virtual void renderObjects() const = 0;

                virtual int getLightCount() const = 0;

                virtual void enableLight(int lightIndex) const = 0;

                virtual math::vec3f getLightPosition(int lightIndex) const = 0;
            };

        public:
            ShadowVolumeRenderer(boost::shared_ptr<ICallback> callback);

            void addOccluder(boost::shared_ptr<Occluder> occluder);

            void renderFrame() const;

        private:
            boost::shared_ptr<ICallback> callback_;

            std::vector<boost::shared_ptr<Occluder>> occluders_;
        };
    }
}

#endif