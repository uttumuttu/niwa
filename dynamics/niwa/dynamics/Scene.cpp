/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "Scene.h"

#include "Cloth.h"
#include "RigidBody.h"

namespace niwa {
    namespace dynamics {
        void Scene::addCloth(boost::shared_ptr<Cloth> cloth) {
            cloths_.push_back(cloth);
        }

        void Scene::addBody(boost::shared_ptr<RigidBody> body) {
            bodies_.push_back(body);
        }

        void Scene::simulate(float deltaSeconds) {
            for(size_t i=0; i<cloths_.size(); ++i) {
                cloths_[i]->simulate(deltaSeconds);
            }

            for(size_t i=0; i<bodies_.size(); ++i) {
                bodies_[i]->getState().simulate(deltaSeconds);
            }
        }
    }
}