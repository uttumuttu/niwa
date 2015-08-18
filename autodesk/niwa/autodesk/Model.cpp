/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/autodesk/Model.h"

#include "niwa/autodesk/Object.h"

#include "niwa/logging/Logger.h"

namespace niwa {
    namespace autodesk {
        using math::vec3f;

        static logging::Logger log(typeid(Model));

        Model::Model() {
            // ignored
        }

        Model::~Model() {
            for(size_t i=0; i<objects_.size(); ++i) {
                delete objects_[i];
                objects_[i] = 0;
            }
        }

        int Model::getObjectCount() const {
            return static_cast<int>(objects_.size());
        }

        Object const& Model::getObjectAt(int index) const {
            return *objects_[index];
        }

        geom::aabb Model::getBounds() const {
            geom::aabb bounds(vec3f(0,0,0));

            bool boundsFound = false;

            for(size_t i=0; i<objects_.size(); ++i) {
                if(objects_[i]->getVertexCount() > 0) {
                    if(boundsFound) {
                        bounds.extendToFit(objects_[i]->getBounds());
                    } else {
                        bounds = objects_[i]->getBounds();
                        boundsFound = true;
                    }
                }
            }

            if(!boundsFound) {
                log.warn() << "Computing bounds of an empty model.";
            }
            return bounds;
        }
    }
}