/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_LEVELSET_OBJECTS_GRID_H
#define NIWA_LEVELSET_OBJECTS_GRID_H

#include "niwa/levelset/ILevelSet.h"

#include "niwa/geom/aabb.h"

#include "niwa/math/vec3f.h"
#include "niwa/math/vec3i.h"

namespace niwa {
    namespace autodesk {
        class Model;
    }

    namespace levelset {
        namespace objects {
            /**
             * An evenly-spaced grid.
             */
            class Grid : public ILevelSet {
            public:
                ~Grid();

            public: // from ILevelSet
                bool isDistanceField() const;

                float __fastcall value(math::vec3f const& position) const;

                math::vec3f __fastcall gradient(math::vec3f const& position) const;

            public: // new functions
                geom::aabb const& getBounds() const;

                math::vec3i const& getDimensions() const;

                math::vec3f const& getVoxelSizes() const;

                /**
                 * @param localPosition Grid position at voxel centers.
                 *
                 * @return World position.
                 */
                __forceinline math::vec3f toWorldCoordinates(
                    math::vec3i const& localPosition) const;

                /**
                 * @param worldPosition World position.
                 *
                 * @return Grid position at voxel centers.
                 */
                __forceinline math::vec3f toLocalCoordinates(
                    math::vec3f const& worldPosition) const;

                __forceinline float elementAt(int x, int y, int z) const;
                __forceinline float& elementAt(int x, int y, int z);

                __forceinline float elementAt(math::vec3i const& vector) const;

                /**
                 * The address of the first grid element (at index (0,0,0))
                 * is guaranteed to be 16-byte aligned.
                 */
                __forceinline float& elementAt(math::vec3i const& vector);

                /**
                 * @todo Can probably be removed and implemented in terms of elementAt.
                 */
                __forceinline int elementIndex(math::vec3i const& position) const;

                /**
                 * Gets the raw grid values. The values
                 * are defined at voxel centers. The value at
                 * voxel center <code>(x,y,z)</code> is 
                 * indexed by <code>(z*ny + y)*nx + x</code>,
                 * where <code>nx</code> and <code>ny</code>
                 * are the dimensions of the grid in x and y directions.
                 *
                 * Guaranteed to be 16-byte aligned.
                 */
                float const* getRaw() const;

                /**
                * Gets the raw grid values. The values
                * are defined at voxel centers. The value at
                * voxel center <code>(x,y,z)</code> is 
                * indexed by <code>(z*ny + y)*nx + x</code>,
                * where <code>nx</code> and <code>ny</code>
                * are the dimensions of the grid in x and y directions.
                *
                * Guaranteed to be 16-byte aligned.
                */
                float* getRaw(); 

                /**
                 * Recomputes the grid values
                 * set to NaN so that the resulting
                 * grid is a proper signed distance field.
                 * (Grid values not set to NaN will serve
                 * as initial values for the algorithm.)
                 */
                void reinitialize();

            private:
                /**
                * @param values_ Passes ownership. Must be aligned at 16 bytes.
                */
                Grid(
                    geom::aabb const& bounds, 
                    math::vec3i const& dimensions,
                    float* values_,
                    bool isDistanceField);

            private: // prevent copying
                Grid(Grid const&);
                Grid& operator = (Grid const&);

            private:
                /**
                 * An axis-aligned bounding box surrounding the grid.
                 */
                const geom::aabb bounds_;

                /**
                 * Grid size in all three dimensions.
                 */
                const math::vec3i dimensions_;

                /**
                 * The signed distance at voxel centers.
                 *
                 * Must be 16-byte aligned.
                 */
                float* const values_; // owned

                const bool isDistanceField_;

                const math::vec3f voxelDimensions_; // auxiliary

                const math::vec3f inverseVoxelDimensions_; // auxiliary

                friend class GridBuilder;
            };
        }
    }
}

#include "Grid.inl"

#endif