/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/levelset/objects/grid.h"

#include "niwa/autodesk/Model.h"
#include "niwa/autodesk/Object.h"

#include <boost/array.hpp>

#include <queue>
#include <limits>

#include <cstdlib>

namespace {
    static inline float isnan(float x) {
        volatile float y = x;
        return y != y;
    }
}

namespace {
    using niwa::math::vec3f;
    using niwa::math::vec3i;

    static inline vec3f itof(vec3i const& vector) {
        return vec3f(
            static_cast<float>(vector[0]), 
            static_cast<float>(vector.y), 
            static_cast<float>(vector.z));
    }

    static inline vec3f elementWiseDivision(vec3f const& lhs, vec3f const& rhs) {
        return vec3f(
            lhs[0] / rhs[0], 
            lhs.y / rhs.y, 
            lhs.z / rhs.z);
    }

    static inline int clamp(int x, int max) {
        if(x >= max) {
            return max-1;
        } else {
            return x;
        }
    }

    static inline int clamp(float x, int max) {
        return clamp(static_cast<int>(x), max);
    }
}

namespace niwa {
    namespace levelset {
        namespace objects {
            using geom::aabb;

            Grid::~Grid() {
                _mm_free(values_);
            }

            bool Grid::isDistanceField() const {
                return isDistanceField_;
            }

            /**
             * Gets the grid value using tri-linear interpolation.
             *
             * If the given position is outside the grid bounds,
             * returns a lower bound for the distance.
             *
             * Acceleration structures were profiled to give
             * only about 10% speedup while taking 10 times
             * more memory, so they aren't used.
             */
            float Grid::value(vec3f const& worldPosition) const {
                vec3i const& n = dimensions_; // shorthand

                vec3f localPosition = toLocalCoordinates(worldPosition);

                float x0 = std::max(0.0f, localPosition[0]);
                float y0 = std::max(0.0f, localPosition.y);
                float z0 = std::max(0.0f, localPosition.z);

                int ix0 = (int)x0;
                int iy0 = (int)y0;
                int iz0 = (int)z0;

                int ix1 = ix0+1;
                int iy1 = iy0+1;
                int iz1 = iz0+1;

                float v000, v100, v010, v110;
                float v001, v101, v011, v111;

                float fx, fy, fz;

                if(ix1 < n[0] && iy1 < n.y && iz1 < n.z) {
                    fx = x0 - ix0;
                    fy = y0 - iy0;
                    fz = z0 - iz0;

                    float* val = &values_[(iz0*n.y + iy0)*n[0] + ix0];
                    v000 = val[0];
                    v100 = val[1];
                    val += n[0];
                    v010 = val[0];
                    v110 = val[1];
                    val += n[0] * n.y;
                    v011 = val[0];
                    v111 = val[1];
                    val -= n[0];
                    v001 = val[0];
                    v101 = val[1];
                } else {
                    if(ix1 >= n[0]) {
                        ix1 = n[0]-1;
                        if(ix0 >= n[0]) {
                            ix0 = n[0]-1;
                        }
                    }
                    if(iy1 >= n.y) {
                        iy1 = n.y-1;
                        if(iy0 >= n.y) {
                            iy0 = n.y-1;
                        }
                    }
                    if(iz1 >= n.z) {
                        iz1 = n.z-1;
                        if(iz0 >= n.z) {
                            iz0 = n.z-1;
                        }
                    }

                    fx = x0 - ix0;
                    fy = y0 - iy0;
                    fz = z0 - iz0;

                    iy0 *= n[0];
                    iy1 *= n[0];

                    iz0 *= n[0] * n.y;
                    iz1 *= n[0] * n.y;

                    v000 = values_[iz0+iy0+ix0];
                    v100 = values_[iz0+iy0+ix1];
                    v010 = values_[iz0+iy1+ix0];
                    v110 = values_[iz0+iy1+ix1];
                    v001 = values_[iz1+iy0+ix0];
                    v101 = values_[iz1+iy0+ix1];
                    v011 = values_[iz1+iy1+ix0];
                    v111 = values_[iz1+iy1+ix1];
                }

                float v00 = v000 + (v100-v000) * fx;
                float v10 = v010 + (v110-v010) * fx;
                float v01 = v001 + (v101-v001) * fx;
                float v11 = v011 + (v111-v011) * fx;

                float v0 = v00 + (v10-v00) * fy;
                float v1 = v01 + (v11-v01) * fy;

                return v0 + (v1-v0) * fz;
            }

            /**
             * Computes the gradient via central differences.
             */
            vec3f Grid::gradient(vec3f const& position) const {
                vec3f const& h = voxelDimensions_; // shorthand

                vec3f hx(h[0],0,0);
                vec3f hy(0,h.y,0);
                vec3f hz(0,0,h.z);

                vec3f gradient(
                    (value(position+hx) - value(position-hx)) / (2 * h[0]),
                    (value(position+hy) - value(position-hy)) / (2 * h.y),
                    (value(position+hz) - value(position-hz)) / (2 * h.z));

                if(isDistanceField_) {
                    gradient.normalize();
                }

                return gradient;
            }

            Grid::Grid(
                    aabb const& bounds, vec3i const& dimensions, 
                    float* values, bool isDistanceField)
                : bounds_(bounds), dimensions_(dimensions),
                  values_(values), isDistanceField_(isDistanceField),
                  voxelDimensions_(
                      elementWiseDivision(bounds_.dimensions(), itof(dimensions_))),
                  inverseVoxelDimensions_(
                      elementWiseDivision(vec3f(1.0,1.0f,1.0f), voxelDimensions_)) {
                assert( !(size_t(static_cast<void*>(values_)) & 15) );
            }

            geom::aabb const& Grid::getBounds() const {
                return bounds_;
            }

            math::vec3i const& Grid::getDimensions() const {
                return dimensions_;
            }

            math::vec3f const& Grid::getVoxelSizes() const {
                return voxelDimensions_;
            }

            float const* Grid::getRaw() const {
                return values_;
            }

            float* Grid::getRaw() {
                return values_;
            }
        }
    }
}

namespace {
    static inline bool isInBounds(
            vec3i const& position, vec3i const& min, vec3i const& max) {
        for(int i=0; i<3; ++i) {
            if(position[i] < min[i] || position[i] >= max[i]) {
                return false;
            }
        }
        return true;
    }
}

namespace niwa { namespace levelset { namespace objects {
    /**
     * The algorithm used is fast marching reinitialization.
     */
    void Grid::reinitialize() {
        struct Element {
            vec3i position;

            float distance;
            float sign;
            float signedDistance;

            Element(vec3i const& position_, float distance_)
                : position(position_), distance(distance_) {
                sign = distance > 0.0f ? 1.0f : -1.0f;

                signedDistance = std::abs(distance);
            }

            bool operator < (Element const& rhs) const {
                return signedDistance > rhs.signedDistance;
            }
        };

        std::priority_queue<Element> elements;

        for(int z=0; z<dimensions_.z; ++z)
        for(int y=0; y<dimensions_.y; ++y)
        for(int x=0; x<dimensions_[0]; ++x) {
            int i = elementIndex(vec3i(x,y,z));

            if(!isnan(values_[i])) {
                elements.push(
                    Element(vec3i(x,y,z),values_[i]));
            }
        }

        if(elements.empty()) {
            int nVoxels = dimensions_[0] * dimensions_.y * dimensions_.z;

            for(int i=0; i<nVoxels; ++i) {
                values_[i] = std::numeric_limits<float>::infinity();
            }
        }

        while(!elements.empty()) {
            Element element = elements.top();
            elements.pop();

            if(element.distance != values_[elementIndex(element.position)]) {
                // invalidated
                continue;
            }

            for(int dz=-1; dz<=1; ++dz)
            for(int dy=-1; dy<=1; ++dy)
            for(int dx=-1; dx<=1; ++dx) {
                if(dx==0 && dy==0 && dz==0) {
                    continue;
                }

                vec3i position = element.position + vec3i(dx,dy,dz);

                if(!isInBounds(position, vec3i(0,0,0), dimensions_)) {
                    continue;
                }

                vec3f direction(
                    voxelDimensions_[0] * dx,
                    voxelDimensions_.y * dy,
                    voxelDimensions_.z * dz);

                float candidateDistance =
                    element.distance 
                    + element.sign * direction.length();

                int i = elementIndex(position);

                if(isnan(values_[i])
                    || (std::abs(candidateDistance) < std::abs(values_[i])
                    && candidateDistance * values_[i] > 0)) {
                    values_[i] = candidateDistance;

                    elements.push(
                        Element(position,candidateDistance));
                }
            }
        }
    }
} } }
