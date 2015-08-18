/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/objects/mesh.h"

#include "niwa/raytrace/objects/Triangle.h"
#include "niwa/raytrace/objects/KdTree.h"

#include "niwa/autodesk/Model.h"
#include "niwa/autodesk/Object.h"

#include "niwa/geom/aabb.h"

#include "niwa/logging/Logger.h"

#include <algorithm>

namespace niwa {
    namespace raytrace {
        namespace objects {
            static logging::Logger Log(typeid(Mesh));

            using math::vec3f;
            using geom::aabb;

            Mesh::Mesh() {
                nVertices_ = 0;
                nFaces_ = 0;

                vertices_ = 0;
                faces_ = 0;

                tree_ = 0;
            }

            Mesh::Mesh(autodesk::Model const& model, aabb const& desiredBounds) {
                nVertices_ = 0;
                nFaces_ = 0;

                Log.debug() << "counting objects";

                for(int i=0; i<model.getObjectCount(); ++i) {
                    autodesk::Object const& object = model.getObjectAt(i);

                    nVertices_ += object.getVertexCount();
                    nFaces_ += object.getFaceCount();
                }

                Log.debug() << "gathering objects";

                vertices_ = new float[3 * nVertices_];
                faces_ = new int[3 * nFaces_];

                int vertexPos = 0;
                int facePos = 0;

                for(int i=0; i<model.getObjectCount(); ++i) {
                    autodesk::Object const& object = model.getObjectAt(i);

                    for(int j=0; j<object.getVertexCount(); ++j) {
                        object.getVertexAt(j, &vertices_[3*vertexPos]);
                        ++vertexPos;
                    }
                    for(int j=0; j<object.getFaceCount(); ++j) {
                        object.getFaceAt(j, &faces_[3*(facePos++)]);
                    }
                }

                Log.debug() << "computing bounds";

                if(nVertices_ > 0) {
                    aabb modelBounds(vec3f(vertices_[0], vertices_[1], vertices_[2]));

                    for(int i=1; i<nVertices_; ++i) {
                        vec3f vertex(vertices_[3*i], vertices_[3*i+1], vertices_[3*i+2]);

                        modelBounds.extendToFit(vertex);
                    }

                    vec3f center = modelBounds.center();

                    float scales[3];

                    for(int i=0; i<3; ++i) {
                        scales[i] = 
                            desiredBounds.dimensions()[i] 
                            / modelBounds.dimensions()[i];
                    }

                    float scale = std::min(scales[0], std::min(scales[1], scales[2]));

                    for(int i=0; i<nVertices_; ++i) {
                        for(int j=0; j<3; ++j) {
                            vertices_[3*i+j] -= modelBounds.center()[j];

                            vertices_[3*i+j] *= scale;

                            vertices_[3*i+j] += desiredBounds.center()[j];
                        }
                    }
                }

                Log.debug() << "computing acceleration structures";

                computeAccelerationStructures();

                Log.debug() << "model ready";
            }

            void Mesh::computeAccelerationStructures() {
                std::vector<vec3f> vertices;

                for(int i=0; i<nFaces_; ++i) {
                    for(int j=0; j<3; ++j) {
                        vertices.push_back(
                            vec3f(
                                vertices_[faces_[3*i+j]*3+0],
                                vertices_[faces_[3*i+j]*3+1],
                                vertices_[faces_[3*i+j]*3+2]));
                    }
                }

                Triangle *const triangles = static_cast<Triangle*>(
                    _mm_malloc(nFaces_ * sizeof(Triangle), 16));

                for(int i=0; i<nFaces_; ++i) {
                    vec3f corners[3];

                    for(int j=0; j<3; ++j) {
                        corners[j] = vertices[3*i+j];
                    }

                    triangles[i] = Triangle(corners);
                }

                tree_ = KdTree::build(nFaces_, triangles, vertices);

                _mm_free(triangles);
            }

            Mesh::~Mesh() {
                delete tree_;
                delete[] vertices_;
                delete[] faces_;
            }

            bool Mesh::raytrace(ray3f const& ray, HitInfo& hitInfo) const {
                if(tree_) {
                    return tree_->raytrace(ray, hitInfo);
                } else {
                    return false;
                }
            }

            bool Mesh::raytraceShadow(ray3f const& ray, float cutoffDistance, ILight const*) const {
                if(tree_) {
                    return tree_->raytraceShadow(ray, cutoffDistance);
                } else {
                    return false;
                }
            }
        }
    }
}