/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "RigidBody.h"

namespace niwa {
    namespace dynamics {
        RigidBody::RigidBody(boost::shared_ptr<RigidBodyCollider> collider) 
            : collider_(collider) {
            // ignored
        }

        RigidBodyState const& RigidBody::getState() const {
            return state_;
        }

        RigidBodyState& RigidBody::getState() {
            return state_;
        }
    }
}