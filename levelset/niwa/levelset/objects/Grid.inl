/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_LEVELSET_OBJECTS_GRID_INL
#define NIWA_LEVELSET_OBJECTS_GRID_INL

namespace niwa {
    namespace levelset {
        namespace objects {
            math::vec3f Grid::toWorldCoordinates(math::vec3i const& localPosition) const {
                math::vec3f const& min = bounds_.minPosition(); // shorthand

                math::vec3f const& h = voxelDimensions_; // shorthand

                return math::vec3f(
                    (localPosition[0] + 0.5f) * h[0] + min[0],
                    (localPosition.y + 0.5f) * h.y + min.y,
                    (localPosition.z + 0.5f) * h.z + min.z);
            }

            math::vec3f Grid::toLocalCoordinates(math::vec3f const& worldPosition) const {
                math::vec3f const& min = bounds_.minPosition(); // shorthand

                math::vec3f const& ih = inverseVoxelDimensions_; // shorthand

                return math::vec3f(
                    (worldPosition[0] - min[0]) * ih[0] - 0.5f,
                    (worldPosition.y - min.y) * ih.y - 0.5f,
                    (worldPosition.z - min.z) * ih.z - 0.5f);
            }

            float Grid::elementAt(int x, int y, int z) const {
                return values_[
                    (z*dimensions_.y + y)*dimensions_[0] + x];
            }

            float& Grid::elementAt(int x, int y, int z) {
                return values_[
                    (z*dimensions_.y + y)*dimensions_[0] + x];
            }

            float Grid::elementAt(math::vec3i const& p) const {
                return values_[
                    (p.z*dimensions_.y + p.y)*dimensions_[0] + p[0]];
            }

            float& Grid::elementAt(math::vec3i const& p) {
                return values_[
                    (p.z*dimensions_.y + p.y)*dimensions_[0] + p[0]];
            }

            int Grid::elementIndex(math::vec3i const& p) const {
                return (p.z * dimensions_.y + p.y) * dimensions_[0] + p[0];
            }
        }
    }
}

#endif