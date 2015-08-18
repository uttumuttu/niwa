/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_DYNAMICS_CLOTHCOLLIDER_H
#define NIWA_DYNAMICS_CLOTHCOLLIDER_H

#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace levelset {
        class ILevelSet;
    }

    namespace dynamics {
        /**
         * An external collision object
         * that cloth collides against.
         *
         * The collision object is assumed
         * to have an infinite mass relative
         * to the cloth; that is, the interaction
         * is one-way.
         */
        class ClothCollider {
        public:
            ClothCollider(
                boost::shared_ptr<levelset::ILevelSet> levelSet,
                float frictionCoefficient);

            levelset::ILevelSet const& levelSet() const;

            float frictionCoefficient() const;

        private:
            boost::shared_ptr<levelset::ILevelSet> levelSet_;

            float frictionCoefficient_;
        };
    }
}

#endif