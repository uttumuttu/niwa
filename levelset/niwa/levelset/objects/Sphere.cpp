/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/levelset/objects/Sphere.h"

namespace niwa {
    namespace levelset {
        namespace objects {
            using math::vec3f;

            Sphere::Sphere() 
                : position_(0,0,0), radius_(1) {
                // ignored
            }

            void Sphere::setPosition(vec3f const& position) {
                position_ = position;
            }

            void Sphere::setRadius(float radius) {
                radius_ = radius;
            }

            float Sphere::value(vec3f const& position) const {
                vec3f delta = position - position_;
                
                float distance = delta.length();

                return distance - radius_;
            }

            vec3f Sphere::gradient(vec3f const& position) const {
                vec3f gradient = position - position_;

                gradient.normalize();

                return gradient;
            }

            bool Sphere::isDistanceField() const {
                return true;
            }
        }
    }
}