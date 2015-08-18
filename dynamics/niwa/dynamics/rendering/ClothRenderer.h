/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DYNAMICS_RENDERING_CLOTHRENDERER_H
#define NIWA_DYNAMICS_RENDERING_CLOTHRENDERER_H

#include <boost/shared_ptr.hpp>

#include "niwa/math/vec3f.h"

#include "niwa/graphics/TriangleStrip.h"

#include <vector>

namespace niwa {
    namespace dynamics {
        class Cloth;

        namespace rendering {
            /**
             * Simple OpenGL-based cloth renderer.
             */
            class ClothRenderer {
            public:
                ClothRenderer(
                    boost::shared_ptr<Cloth> cloth);

                /**
                 * Precomputes rendering a frame.
                 * Has to be called before invoking
                 * render if there has been any change
                 * in the state of the cloth.
                 */
                void precomputeFrame() const;

                void render() const;

            private:
                boost::shared_ptr<Cloth> cloth_;

                std::vector<graphics::TriangleStrip> strips_;

                mutable std::vector<math::vec3f> vertexNormals_;
            };
        }
    }
}

#endif