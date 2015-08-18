/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "RigidBodyState.h"

#include <cassert>

namespace {
    using niwa::math::mat3f;
    using niwa::math::vec3f;

    /**
     * @return For given vector \f$a\f$ returns an antisymmetric matrix
     *         \f$A\f$ such that \f$Ab = a \cross b\f$ for all vectors \f$b\f.
     */
    static mat3f star(vec3f const& vec) {
        mat3f mat(mat3f::Uninitialized);

        mat[0][0] = 0; mat[0][1] = -vec.z; mat[0][2] = vec.y;
        mat[1][0] = vec.z; mat[1][1] = 0; mat[1][2] = -vec.x;
        mat[2][0] = -vec.y; mat[2][1] = vec.x; mat[2][2] = 0;

        assert(mat.isAntiSymmetric());

        return mat;
    }

    /**
     * Orthogonalizes the given matrix with
     * the Gram--Schmidt method. If the process
     * fails, the matrix is left untouched.
     *
     * @return Whether the orthogonalization was a success.
     */
    static bool orthogonalize(mat3f& mat) {
        vec3f a(mat[0][0], mat[1][0], mat[2][0]);
        vec3f b(mat[0][1], mat[1][1], mat[2][1]);

        if(a.length() == 0) {
            return false;
        } else {
            a.normalize();
        }

        b -= a * vec3f::dot(a,b);

        if(b.length() == 0) {
            return false;
        } else {
            b.normalize();
        }

        vec3f c = vec3f::cross(a, b);

        mat[0][0] = a.x; mat[1][0] = a.y; mat[2][0] = a.z;
        mat[0][1] = b.x; mat[1][1] = b.y; mat[2][1] = b.z;
        mat[0][2] = c.x; mat[1][2] = c.y; mat[2][2] = c.z;
    }
}

namespace niwa {
    namespace dynamics {
        RigidBodyState::RigidBodyState()
            : mass_(1.0f), localInertia_(mat3f::Identity),
              orientation_(mat3f::Identity) {
            // ignored
        }

        /**
         * Simulates rigid body time evolution
         * with Euler--Cromer method.
         */
        void RigidBodyState::simulate(float timeSeconds) {
            simulateTranslational(timeSeconds);

            simulateRotational(timeSeconds);
        }

        void RigidBodyState::simulateTranslational(float timeSeconds) {
            linearMomentum_ += force_ * timeSeconds;

            vec3f velocity = linearMomentum_ / mass_;

            position_ += velocity * timeSeconds;
        }

        void RigidBodyState::simulateRotational(float timeSeconds) {
            angularMomentum_ += torque_ * timeSeconds;

            mat3f invWorldInertia = 
                orientation_ * inv(localInertia_) * trn(orientation_);

            vec3f angularVelocity = invWorldInertia * angularMomentum_;

            mat3f oldOrientation = orientation_;

            orientation_ += star(angularVelocity) * orientation_ * timeSeconds;

            if(!orthogonalize(orientation_)) {
                orientation_ = oldOrientation;
            }
        }

        vec3f RigidBodyState::toWorldSpace(vec3f const& localPosition) const {
            return orientation_ * localPosition + position_;
        }

        vec3f RigidBodyState::toLocalSpace(vec3f const& worldPosition) const {
            return (worldPosition - position_) * orientation_;
        }
    }
}