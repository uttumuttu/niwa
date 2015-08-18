/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "RigidBodyCollider.h"

namespace niwa {
    namespace dynamics {
        RigidBodyCollider::RigidBodyCollider() 
            : frictionCoefficient_(0),
            restitutionCoefficient_(0) {
            // ignored
        }

        /** @todo */
        RigidBodyCollider* RigidBodyCollider::create(niwa::autodesk::Model const& /*model*/) {
            return 0;
        }

        float RigidBodyCollider::getFrictionCoefficient() const {
            return frictionCoefficient_;
        }

        float RigidBodyCollider::getRestitutionCoefficient() const {
            return restitutionCoefficient_;
        }

        void RigidBodyCollider::setFrictionCoefficient(float value) {
            frictionCoefficient_ = value;
        }

        void RigidBodyCollider::setRestitutionCoefficient(float value) {
            restitutionCoefficient_ = value;
        }
    }
}