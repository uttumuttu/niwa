/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DYNAMICS_SCENE_H
#define NIWA_DYNAMICS_SCENE_H

#include <vector>
#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace dynamics {
        class Cloth;
        class RigidBody;

        class Scene {
        public:
            void addCloth(boost::shared_ptr<Cloth> cloth);

            void addBody(boost::shared_ptr<RigidBody> body);

            void simulate(float deltaSeconds);

        private:
            std::vector<boost::shared_ptr<Cloth>> cloths_;

            std::vector<boost::shared_ptr<RigidBody>> bodies_;
        };
    }
}

#endif