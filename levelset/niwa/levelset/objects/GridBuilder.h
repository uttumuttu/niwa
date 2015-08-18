/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_LEVELSET_OBJECTS_GRIDBUILDER_H
#define NIWA_LEVELSET_OBJECTS_GRIDBUILDER_H

#include "niwa/geom/aabb.h"
#include "niwa/math/vec3i.h"

namespace niwa {
    namespace autodesk {
        class Model;
    }

    namespace levelset {
        class ILevelSet;

        namespace objects {
            class Grid;

            /**
             * Builds level set grids.
             */
            class GridBuilder {
            public:
                GridBuilder(
                    geom::aabb const& desiredBounds,
                    math::vec3i const& dimensions);

                /**
                 * Constructs an empty field
                 * (all values set to positive infinity).
                 */
                Grid* buildEmpty() const;

                /**
                 * Constructs a signed distance field
                 * for the given model.
                 */
                Grid* build(autodesk::Model const& model) const;

                /**
                 * Constructs a grid by sampling
                 * the underlying model in the given
                 * bounds.
                 */
                Grid* build(ILevelSet const& levelSet) const;

            private: // prevent copying
                GridBuilder(GridBuilder const&);
                GridBuilder& operator = (GridBuilder const&);

            private:
                const geom::aabb desiredBounds_;

                const math::vec3i dimensions_;
            };
        }
    }
}

#endif