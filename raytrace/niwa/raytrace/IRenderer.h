/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_IRENDERER_H
#define NIWA_RAYTRACE_IRENDERER_H

namespace niwa {
    namespace raytrace {
        /**
         * Renders scenes. The source (scene and lighting)
         * and destination (e.g., OpenGL or filesystem)
         * is left up to implementations.
         */
        class IRenderer {
        public:
            virtual ~IRenderer();

            /**
             * @return Whether the rendering was successful.
             */
            virtual bool render() const = 0;
        };
    }
}

#endif