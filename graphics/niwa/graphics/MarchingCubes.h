/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_GRAPHICS_MARCHINGCUBES_H
#define NIWA_GRAPHICS_MARCHINGCUBES_H

#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace levelset {
        namespace objects {
            class Grid;
        }
    }

    namespace graphics {
        /**
         * Three-dimensional isosurface contouring
         * with the marching cubes algorithm.
         */
        class MarchingCubes {
        public:
            MarchingCubes();

            void setUseOpenGl(bool useOpenGl);

            bool getUseOpenGl() const;

            /** Default value true. */
            void setUseVertexNormals(bool value);

            bool getUseVertexNormals() const;

            /**
             * Renders the given grid with OpenGL.
             */
            void render(
                levelset::objects::Grid const& grid) const;

        private:
            bool useOpenGl_;

            bool useVertexNormals_;
        };
    }
}

#endif