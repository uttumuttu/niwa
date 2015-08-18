/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DYNAMICS_RIGIDBODYCOLLIDER_H
#define NIWA_DYNAMICS_RIGIDBODYCOLLIDER_H

#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace autodesk {
        class Model;
    }

    namespace levelset {
        class ILevelSet;
    }

    namespace dynamics {
        class RigidBodyCollider {
        public:
            static RigidBodyCollider* create(
                autodesk::Model const& model);

            float getFrictionCoefficient() const;

            float getRestitutionCoefficient() const;

            /** @param value Non-negative. */
            void setFrictionCoefficient(float value);

            /** @param value In [0,1] (inclusive). */
            void setRestitutionCoefficient(float value);

        private:
            RigidBodyCollider();

        private:
            float frictionCoefficient_;

            float restitutionCoefficient_;

            boost::shared_ptr<levelset::ILevelSet> levelSet_;
        };
    }
}

#endif