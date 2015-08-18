/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_DYNAMICS_CLOTHPARTICLE_H
#define NIWA_DYNAMICS_CLOTHPARTICLE_H

#include "niwa/math/vec3f.h"

namespace niwa {
    namespace dynamics {
        class ClothParticle {
        public:
            ClothParticle(float mass_, math::vec3f const& position);

            math::vec3f const& getPosition() const;

        private:
            void setMass(float mass);

        private:
            math::vec3f position_;
            math::vec3f velocity_;

            float mass_;
            float invMass_;

            friend class Cloth;
            friend class ClothBuilder;
            friend class ClothPackedSpring;
            friend class ClothSpring;
        };
    }
}

#endif