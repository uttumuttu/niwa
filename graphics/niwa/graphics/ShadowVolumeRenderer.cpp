/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "ShadowVolumeRenderer.h"

#include "Occluder.h"

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

#include <cmath>

namespace niwa {
    namespace graphics {
        using math::vec3f;

        ShadowVolumeRenderer::ICallback::~ICallback() {
            // ignored
        }

        ShadowVolumeRenderer::ShadowVolumeRenderer(boost::shared_ptr<ICallback> callback) 
            : callback_(callback) {
            // ignored
        }

        void ShadowVolumeRenderer::addOccluder(boost::shared_ptr<Occluder> occluder) {
            occluders_.push_back(occluder);
        }

        void ShadowVolumeRenderer::renderFrame() const {
            // Step one.
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            
            // Step three skipped (we assume the modelview matrix is set).

            // Step four.
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_LIGHTING);

            callback_->disableLights();

            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, callback_->getAmbientLight());

            callback_->renderObjects();

            const float zero[4] = {0,0,0,0};

            // Step five.
            glDepthMask(0);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, zero);

            // Step six (for each light source).

            for(int light=0; light<callback_->getLightCount(); ++light) {
                glClear(GL_STENCIL_BUFFER_BIT);

                glColorMask(0,0,0,0);
                glEnable(GL_STENCIL_TEST);
                glStencilFunc(GL_ALWAYS, 0, ~0U);
                glStencilMask(~0U);

                glEnable(GL_CULL_FACE);

                const vec3f lightPosition = callback_->getLightPosition(light);

                for(size_t i=0; i < occluders_.size(); ++i) {
                    Occluder const& occluder = *occluders_[i];

                    occluder.updateFacings(lightPosition);

                    for(int pass=0; pass<2; ++pass) {
                        glCullFace(!pass ? GL_FRONT : GL_BACK);
                        glStencilOp(GL_KEEP, !pass ? GL_INCR : GL_DECR, GL_KEEP);

                        occluder.renderSilhouette(lightPosition);
                    }
                }

                callback_->enableLight(light);

                glStencilFunc(GL_EQUAL, 0, ~0U);
                glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
                glDepthFunc(GL_EQUAL);
                glColorMask(1,1,1,1);

                callback_->renderObjects();

                glDepthFunc(GL_LESS);
            }

            // Step seven (cleanup).
            glDisable(GL_BLEND);
            glDisable(GL_STENCIL_TEST);
            glDepthMask(1);
        }
    }
}