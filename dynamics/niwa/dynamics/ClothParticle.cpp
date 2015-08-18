/**
 * @file 
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "ClothParticle.h"

namespace niwa {
    namespace dynamics {
        using math::vec3f;

        ClothParticle::ClothParticle(float mass, math::vec3f const& position) 
            : mass_(mass), invMass_(1 / mass), position_(position) {
                // ignored
        }

        void ClothParticle::setMass(float mass) {
            mass_ = mass;
            invMass_ = 1 / mass_;
        }

        vec3f const& ClothParticle::getPosition() const {
            return position_;
        }
    }
}