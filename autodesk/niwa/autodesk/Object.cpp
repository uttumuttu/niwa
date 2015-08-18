/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/autodesk/Object.h"

#include "niwa/logging/Logger.h"

namespace niwa {
    namespace autodesk {
        using math::vec3f;

        static logging::Logger log(typeid(Object));

        Object::Object(std::string const& name) 
            : name_(name), nVertices_(0), nFaces_(0),
              vertices_(0), faces_(0) {
            // ignored
        }

        Object::~Object() {
            delete[] vertices_;
            delete[] faces_;
        }

        std::string const& Object::getName() const {
            return name_;
        }

        int Object::getVertexCount() const {
            return nVertices_;
        }

        int Object::getFaceCount() const {
            return nFaces_;
        }

        void Object::getVertexAt(int index, float* out) const {
            out[0] = vertices_[3*index];
            out[1] = vertices_[3*index+1];
            out[2] = vertices_[3*index+2];
        }

        void Object::getFaceAt(int index, int* out) const {
            out[0] = faces_[3*index];
            out[1] = faces_[3*index+1];
            out[2] = faces_[3*index+2];
        }

        geom::aabb Object::getBounds() const {
            geom::aabb bounds(
                vec3f(
                    vertices_[0], vertices_[1], vertices_[2]));

            for(int i=1; i<nVertices_; ++i) {
                bounds.extendToFit(
                    vec3f(
                        vertices_[3*i], vertices_[3*i+1], vertices_[3*i+2]));
            }

            return bounds;
        }
    }
}