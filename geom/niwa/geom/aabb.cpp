/**
 * @file
 * @author Mikko Kauppila
 * 
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/geom/aabb.h"

#include <algorithm>

namespace niwa {
    namespace geom {
        using math::vec3f;

        aabb::aabb(vec3f const& vector)
                : minPosition_(vector), maxPosition_(vector) {
            // ignored
        }

        aabb::aabb(vec3f const& minPosition, vec3f const& maxPosition) 
                : minPosition_(minPosition), maxPosition_(maxPosition) {
            // ignored
        }

        void aabb::extendToFit(vec3f const& vector) {
            for(int i=0; i<3; ++i) {
                minPosition_[i] = std::min(minPosition_[i], vector[i]);
                maxPosition_[i] = std::max(maxPosition_[i], vector[i]);
            }
        }

        void aabb::extendDimensionsBy(vec3f const& vector) {
            minPosition_ -= vector;
            maxPosition_ += vector;
        }

        void aabb::extendDimensionsBy(float value) {
            extendDimensionsBy(vec3f(value, value, value));
        }

        void aabb::extendToFit(aabb const& bounds) {
            for(int i=0; i<3; ++i) {
                minPosition_[i] = std::min(
                    minPosition_[i], bounds.minPosition_[i]);

                maxPosition_[i] = std::max(
                    maxPosition_[i], bounds.maxPosition_[i]);
            }
        }

        vec3f const& aabb::minPosition() const {
            return minPosition_;
        }

        vec3f const& aabb::maxPosition() const {
            return maxPosition_;
        }

        vec3f aabb::center() const {
            return (minPosition_ + maxPosition_) / 2;
        }

        vec3f aabb::dimensions() const {
            return maxPosition_ - minPosition_;
        }

        vec3f const* aabb::getExtrema() const {
            return &minPosition_;
        }

        bool aabb::contains(vec3f const& position) const {
            return minPosition_[0] <= position[0] &&
                minPosition_.y <= position.y &&
                minPosition_.z <= position.z &&
                position[0] <= maxPosition_[0] &&
                position.y <= maxPosition_.y &&
                position.z <= maxPosition_.z;
        }

        vec3f aabb::closestPositionTo(vec3f const& position) const {
            return vec3f(
                std::max(minPosition_[0], std::min(maxPosition_[0], position[0])),
                std::max(minPosition_.y, std::min(maxPosition_.y, position.y)),
                std::max(minPosition_.z, std::min(maxPosition_.z, position.z)));
        }

        float aabb::shortestDistanceTo(vec3f const& position) const {
            vec3f delta = position - closestPositionTo(position);

            return delta.length();
        }
    }
}