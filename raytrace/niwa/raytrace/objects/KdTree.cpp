/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/objects/KdTree.h"

#include "niwa/raytrace/objects/Triangle.h"

#include "niwa/raytrace/ray3f.h"
#include "niwa/raytrace/Constants.h"

#include "niwa/geom/aabb.h"

#include <algorithm>

#define MAX_DEPTH 20
#define MAX_LEAF_TRIANGLES 32

namespace {
    static inline int argmin(int n, float* xs) {
        float xmin = xs[0];
        int imin = 0;

        for(int i=1; i<n; ++i) {
            if(xs[i] < xmin) {
                xmin = xs[i];
                imin = i;
            }
        }

        return imin;
    }

    static inline int argmax(int n, float* xs) {
        float xmax = xs[0];
        int imax = 0;

        for(int i=1; i<n; ++i) {
            if(xs[i] > xmax) {
                xmax = xs[i];
                imax = i;
            }
        }

        return imax;
    }
}

#define USE_SURFACE_AREA_HEURISTIC 0

namespace {
    using niwa::raytrace::objects::Triangle;
    using niwa::geom::aabb;
    using niwa::math::vec3f;

#if USE_SURFACE_AREA_HEURISTIC
    float maximumTrianglePosition(vec3f const vertices[3], int dimension) {
        return std::max(
            vertices[0][dimension],
            std::max(
                vertices[1][dimension],
                vertices[2][dimension]));
    }

    float triangleArea(vec3f const vertices[3]) {
        vec3f edges[2] = {
            vertices[1] - vertices[0],
            vertices[2] - vertices[0]
        };
        return vec3f::cross(edges[0], edges[1]).length() / 2;
    }

    /**
     * @return A pair consisting of the surface area metric (estimated traversal cost)
     *         and the split position associated with it.
     */
    static std::pair<float,float> surfaceAreaMetric(
        std::vector<vec3f> const& baseVertices,
        std::vector<size_t> const& activeTriangles, size_t splitDimension) {
        struct Node {
            Node(float position_, float area_)
                : position(position_), area(area_) {
                    // ignored
            }

            float position;
            float area;

            bool operator < (Node const& rhs) const {
                return position < rhs.position;
            }
        };

        std::vector<Node> nodes;

        float totalArea = 0;

        for(size_t i=0; i<activeTriangles.size(); ++i) {
            size_t triangle = activeTriangles[i];

            // When splitting according to this position,
            // the triangle is guaranteed to be on the left
            // side of the tree.
            float position = maximumTrianglePosition(&baseVertices[3*triangle], splitDimension);
            float area = triangleArea(&baseVertices[3*triangle]);

            totalArea += area;

            nodes.push_back(Node(position, area));
        }

        std::sort(nodes.begin(), nodes.end());

        float leftArea = 0;

        float optimalCost = 0;
        int optimalNode = -1;

        for(size_t i=0; i<nodes.size(); ++i) {
            // If we split according to nodes[i].position,
            // the triangle is guaranteed to be on the
            // left side of the tree.
            leftArea += nodes[i].area;

            float rightArea = totalArea - leftArea;

            size_t nLeftObjects = i+1;
            size_t nRightObjects = nodes.size() - nLeftObjects;

            float cost = leftArea * nLeftObjects + rightArea * nRightObjects;

            if(optimalNode == -1 || cost < optimalCost) {
                optimalNode = i;
                optimalCost = cost;
            }
        }

        return std::pair<float,float>(optimalCost, nodes[optimalNode].position);
    }
#endif

    static int getSplitDimension(
            std::vector<vec3f> const& /*baseVertices*/,
            std::vector<size_t> const& /*activeTriangles*/, int depth) {
        return depth % 3;
    }

#if USE_SURFACE_AREA_HEURISTIC
    static float getSplitPosition(
        int splitDimension,
        std::vector<vec3f> const& baseVertices,
        std::vector<size_t> const& activeTriangles, aabb const& /*bounds*/) {
        return surfaceAreaMetric(baseVertices, activeTriangles, splitDimension).second;
    }
#else
    static float getSplitPosition(
        int splitDimension,
        std::vector<vec3f> const& /*baseVertices*/,
        std::vector<size_t> const& /*activeTriangles*/, aabb const& bounds) {
        return bounds.center()[splitDimension];
    }
#endif
}

namespace {
    using niwa::geom::aabb;
    using niwa::math::packed_vec3f;
    using niwa::math::packed_vec3i;
    using niwa::math::vec3f;
    using niwa::math::vec3i;
    using niwa::raytrace::packed_ray3f;
    using niwa::raytrace::ray3f;

    /**
     * @param bounds Assumed to have been extended to control for rounding errors.
     */
    static bool overlaps(aabb const& bounds, ray3f const& ray) {
        vec3f const* elt = bounds.getExtrema();

        // Algorithm based on the 2002 paper
        // "An Efficient and Robust Ray-Box Intersection Algorithm"
        // by Amy Williams, Steve Barrus, R. Keith Morley and Peter Shirley.

        vec3f const& pos = ray.getPosition();
        vec3i const& sgn = ray.getSigns();
        vec3f const& inv = ray.getDirectionInverses();

        float tmin  = (elt[  sgn[0]].x - pos.x) * inv[0];
        float tmax  = (elt[1-sgn[0]].x - pos.x) * inv[0];
        float tymin = (elt[  sgn[1]].y - pos.y) * inv[1];
        float tymax = (elt[1-sgn[1]].y - pos.y) * inv[1];

        if( (tmin>tymax) || (tymin>tmax) ) {
            // intervals are distinct
            return false;
        }
        if(tymin>tmin) {
            // unify x-y intervals
            tmin = tymin;
        }
        if(tymax<tmax) {
            tmax = tymax;
        }

        float tzmin = (elt[  sgn[2]].z - pos.z) * inv[2];
        float tzmax = (elt[1-sgn[2]].z - pos.z) * inv[2];

        if( (tmin>tzmax) || (tzmin>tmax) ) {
            // intervals are distinct
            return false;
        }
        //if(tzmin>tmin) { // no unification necessary
            //tmin = tzmin;
        //}
        if(tzmax<tmax) {
            tmax = tzmax;
        }
        return tmax>0;
    }
}

namespace niwa {
    namespace raytrace {
        namespace objects {
            using geom::aabb;
            using math::vec3f;

            KdTree::~KdTree() {
                if(triangles_) {
                    _mm_free(const_cast<Triangle*>(triangles_));
                }
                delete left_;
                delete right_;
            }

            KdTree* KdTree::build(
                    size_t nBaseTriangles,
                    Triangle const*const baseTriangles,
                    std::vector<vec3f> const& baseVertices) {
                std::vector<size_t> activeTriangles;

                for(size_t i=0; i<nBaseTriangles; ++i) {
                    activeTriangles.push_back(i);
                }

                return build(baseTriangles, baseVertices, activeTriangles, 0);
            }

            KdTree* KdTree::build(
                    Triangle const*const baseTriangles,
                    std::vector<vec3f> const& baseVertices,
                    std::vector<size_t> const& activeTriangles, int depth) {
                const size_t nActiveTriangles = activeTriangles.size();

                if(nActiveTriangles == 0) {
                    return new KdTree(aabb(vec3f(0,0,0)), 0, 0);
                }

                aabb bounds( baseVertices[3*activeTriangles[0]] );

                for(size_t i=0; i<nActiveTriangles; ++i) {
                    for(int j=0; j<3; ++j) {
                        bounds.extendToFit( baseVertices[3*activeTriangles[i]+j] );
                    }
                }

                bounds.extendDimensionsBy(constants::DISTANCE_EPSILON);

                if(depth >= MAX_DEPTH || nActiveTriangles <= MAX_LEAF_TRIANGLES) {
                    Triangle* const leafTriangles = static_cast<Triangle*>(
                        _mm_malloc(nActiveTriangles * sizeof(Triangle), 16));

                    for(size_t i=0; i<nActiveTriangles; ++i) {
                        leafTriangles[i] = baseTriangles[activeTriangles[i]];
                    }

                    return new KdTree(bounds, nActiveTriangles, leafTriangles);
                }

                const int splitDimension = getSplitDimension(
                    baseVertices, activeTriangles, depth);

                const float splitPosition = getSplitPosition(
                    splitDimension, baseVertices, activeTriangles, bounds);

                std::vector<size_t> leftTriangles;
                std::vector<size_t> rightTriangles;

                for(size_t i=0; i<nActiveTriangles; ++i) {
                    bool onLeft = false;
                    bool onRight = false;

                    for(int j=0; j<3; ++j) {
                        vec3f const& vertex = baseVertices[3*activeTriangles[i]+j];

                        if(vertex[splitDimension] <= splitPosition) {
                            onLeft = true;
                        } else {
                            onRight = true;
                        }
                    }

                    if(onLeft) {
                        leftTriangles.push_back(activeTriangles[i]);
                    }
                    if(onRight) {
                        rightTriangles.push_back(activeTriangles[i]);
                    }
                }

                KdTree* left(
                    build(baseTriangles, baseVertices, leftTriangles, 1+depth));

                KdTree* right(
                    build(baseTriangles, baseVertices, rightTriangles, 1+depth));

                return new KdTree(bounds, splitDimension, splitPosition, left, right);
            }

            KdTree::KdTree(aabb const& bounds, size_t nTriangles, Triangle const*const triangles)
                : isLeaf_(true), bounds_(bounds), nTriangles_(nTriangles), triangles_(triangles),
                  splitDimension_(0), splitPosition_(0), left_(0), right_(0) {
                // ignored
            }

            KdTree::KdTree(
                    aabb const& bounds, int splitDimension, float splitPosition, 
                    KdTree const* left, KdTree const* right)
                : isLeaf_(false), bounds_(bounds), nTriangles_(0), triangles_(0),
                  splitDimension_(splitDimension), splitPosition_(splitPosition),
                  left_(left), right_(right) {
                // ignored
            }

            bool KdTree::raytrace(ray3f const& ray, HitInfo& hitInfo) const {
                bool hitFound = false;

                raytrace(ray, hitInfo, hitFound);

                return hitFound;
            }

            bool KdTree::raytraceShadow(ray3f const& ray, float cutoffDistance) const {
                jmp_buf buffer;

#pragma warning(disable:4611)
                if(setjmp(buffer)) {
                    return true;
                }

                raytraceShadow(ray, cutoffDistance, buffer);

                return false;
            }

            void KdTree::raytrace(
                    ray3f const& ray, HitInfo& hitInfo, bool& hitFound) const {
                if(!overlaps(bounds_, ray)) {
                    return;
                }

                if(isLeaf_) {
                    const size_t n = nTriangles_;

                    Triangle const* const triangles = triangles_;

                    for(size_t i=0; i<n; ++i) {
                        triangles[i].raytrace(ray, hitInfo, hitFound);
                    }
                } else {
                    float pos = ray.getPosition()[splitDimension_];

                    if(pos <= splitPosition_) {
                        if(pos <= splitPosition_ - constants::DISTANCE_EPSILON
                            && ray.getDirection()[splitDimension_] <= 0) {
                            left_->raytrace(ray, hitInfo, hitFound);
                        } else {
                            left_->raytrace(ray, hitInfo, hitFound);

                            if(!hitFound) {
                                right_->raytrace(ray, hitInfo, hitFound);
                            }
                        }
                    } else {
                        if(pos >= splitPosition_ + constants::DISTANCE_EPSILON
                            && ray.getDirection()[splitDimension_] >= 0) {
                            right_->raytrace(ray, hitInfo, hitFound);
                        } else {
                            right_->raytrace(ray, hitInfo, hitFound);

                            if(!hitFound) {
                                left_->raytrace(ray, hitInfo, hitFound);
                            }
                        }
                    }
                }
            }

            void KdTree::raytraceShadow(
                ray3f const& ray, float cutoffDistance, jmp_buf& jump) const {
                if(!overlaps(bounds_, ray)) {
                    return;
                }

                if(isLeaf_) {
                    const size_t n = nTriangles_;

                    Triangle const* const triangles = triangles_;

                    for(size_t i=0; i<n; ++i) {
                        triangles[i].raytraceShadow(ray, cutoffDistance, jump);
                    }
                } else {
                    float pos = ray.getPosition()[splitDimension_];

                    if(pos <= splitPosition_) {
                        if(pos <= splitPosition_ - constants::DISTANCE_EPSILON
                            && ray.getDirection()[splitDimension_] <= 0) {
                            left_->raytraceShadow(ray, cutoffDistance, jump);
                        } else {
                            left_->raytraceShadow(ray, cutoffDistance, jump);

                            right_->raytraceShadow(ray, cutoffDistance, jump);
                        }
                    } else {
                        if(pos >= splitPosition_ + constants::DISTANCE_EPSILON
                            && ray.getDirection()[splitDimension_] >= 0) {
                            right_->raytraceShadow(ray, cutoffDistance, jump);
                        } else {
                            right_->raytraceShadow(ray, cutoffDistance, jump);

                            left_->raytraceShadow(ray, cutoffDistance, jump);
                        }
                    }
                }
            }
        } // namespace objects
    } // namespace raytrace
} // namespace niwa