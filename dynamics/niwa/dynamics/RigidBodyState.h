/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DYNAMICS_RIGIDBODYSTATE_H
#define NIWA_DYNAMICS_RIGIDBODYSTATE_H

#include "niwa/math/mat3f.h"
#include "niwa/math/vec3f.h"

namespace niwa {
    namespace dynamics {
        /**
         * The time-dependent state of a rigid body.
         */
        class RigidBodyState {
        public:
            RigidBodyState();

            void simulate(float timeSeconds);

        private:
            math::vec3f toWorldSpace(math::vec3f const& localPosition) const;

            math::vec3f toLocalSpace(math::vec3f const& worldPosition) const;

        private:
            void simulateTranslational(float timeSeconds);

            void simulateRotational(float timeSeconds);

        private:
            // inertial variables
            float mass_;
            math::mat3f localInertia_;

            // spatial variables
            math::vec3f position_;
            math::mat3f orientation_;

            // momentum variables
            math::vec3f linearMomentum_;
            math::vec3f angularMomentum_;

            // force and torque
            math::vec3f force_;
            math::vec3f torque_;
        };
    }
}

#endif