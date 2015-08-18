/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DYNAMICS_RIGIDBODY_H
#define NIWA_DYNAMICS_RIGIDBODY_H

#include "RigidBodyState.h"

#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace dynamics {
        class RigidBodyCollider;

        class RigidBody {
        public:
            explicit RigidBody(boost::shared_ptr<RigidBodyCollider> collider);

            RigidBodyState const& getState() const;

            RigidBodyState& getState();

        private:
            RigidBodyState state_;

            boost::shared_ptr<RigidBodyCollider> collider_;
        };
    }
}

#endif