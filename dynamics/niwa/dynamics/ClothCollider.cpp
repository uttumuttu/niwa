/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "ClothCollider.h"

namespace niwa {
    namespace dynamics {
        ClothCollider::ClothCollider(
            boost::shared_ptr<levelset::ILevelSet> levelSet,
            float frictionCoefficient)
            : levelSet_(levelSet), frictionCoefficient_(frictionCoefficient) {
                // ignored
        }

        levelset::ILevelSet const& ClothCollider::levelSet() const {
            return *levelSet_;
        }

        float ClothCollider::frictionCoefficient() const {
            return frictionCoefficient_;
        }
    }
}