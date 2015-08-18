/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_OBJECTS_KDTREE_H
#define NIWA_RAYTRACE_OBJECTS_KDTREE_H

#include <vector>

#include "niwa/geom/aabb.h"

#include <setjmp.h>

namespace niwa {
    namespace math {
        class vec3f;
    }

    namespace raytrace {
        class ray3f;
        class HitInfo;

        namespace objects {
            class Triangle;

            /**
             * A bounding volume hierarchy for triangular meshes.
             */
            class KdTree {
            public:
                /**
                 * @param baseTriangles Ownership is not passed.
                 *
                 * @return The resulting KD-tree, never null.
                 */
                static KdTree* build(
                    size_t nBaseTriangles,
                    Triangle const*const baseTriangles,
                    std::vector<math::vec3f> const& baseVertices);

                bool __fastcall raytrace(ray3f const& ray, HitInfo& hitInfo) const;

                bool __fastcall raytraceShadow(ray3f const& ray, float cutoffDistance) const;

                ~KdTree();

            private:
                /**
                 * Constructs a leaf node.
                 *
                 * @param bounds An axis-aligned bounding box covering the triangles in the node.
                 *
                 * @param triangles Passes ownership.
                 */
                KdTree(geom::aabb const& bounds, 
                    size_t nTriangles,
                    Triangle const*const triangles);

                /**
                 * Constructs a non-leaf node.
                 */
                KdTree(geom::aabb const& bounds, int splitDimension, float splitPosition, 
                    KdTree const* left, KdTree const* right);

            private:
                static KdTree* build(
                    Triangle const*const baseTriangles,
                    std::vector<math::vec3f> const& baseVertices,
                    std::vector<size_t> const& activeTriangles, int depth);

                void __fastcall raytrace(
                    ray3f const& ray, HitInfo& hitInfo, bool& hitFound) const;

                void __fastcall raytraceShadow(
                    ray3f const& ray, float cutoffDistance,
                    jmp_buf& jump) const;

            private: // prevent copying
                KdTree(KdTree const&);
                KdTree& operator = (KdTree const&);

            private:
                const bool isLeaf_;

                const geom::aabb bounds_;

                /**
                 * Used only for leaf nodes.
                 */
                const size_t nTriangles_;

                /**
                 * Used only for leaf nodes.
                 */
                Triangle const*const triangles_;

                /**
                 * Only used for non-leaf nodes.
                 */
                const int splitDimension_;

                /**
                 * Only used for non-leaf nodes. Owned.
                 */
                const float splitPosition_;

                /**
                 * Only used for non-leaf nodes, owned.
                 */
                KdTree const* const left_;

                /**
                 * Only used for non-leaf nodes, owned.
                 */
                KdTree const* const right_;
            };
        }
    }
}

#endif