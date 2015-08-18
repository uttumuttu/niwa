/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "Occluder.h"

#include "niwa/logging/Logger.h"

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

#include <cassert>

namespace niwa {
    namespace graphics {
        static logging::Logger log(typeid(Occluder));

        using math::vec3f;

        std::vector<math::vec3f> const& Occluder::getVertices() const {
            return vertices_;
        }

        std::vector<math::vec3f>& Occluder::getVertices() {
            return vertices_;
        }

        void Occluder::updateFacings(vec3f const& lightPosition) const {
            size_t nTriangles = triangles_.size();

            if(nWingedEdges_ > 0) {
                nTriangles /= 2;
            }

            for(size_t i=0; i<nTriangles; ++i) {
                Triangle const& triangle = triangles_[i];

                vec3f normal = vec3f::cross(
                    vertices_[triangle.indices[1]] - vertices_[triangle.indices[0]],
                    vertices_[triangle.indices[2]] - vertices_[triangle.indices[0]]);

                triangle.isBackFacing = vec3f::dot(
                    normal, lightPosition - vertices_[triangle.indices[0]]) < 0;
            }

            if(nWingedEdges_ > 0) {
                for(size_t i=0; i<nTriangles; ++i) {
                    triangles_[nTriangles + i].isBackFacing = !triangles_[i].isBackFacing;
                }
            }
        }

        /**
         * @todo Currently the cap rendering is the bottleneck.
         *       Its speed can be improved drastically by
         *       real-time triangle strip generation.
         */
        void Occluder::renderSilhouette(vec3f const& lightPosition) const {
            glBegin(GL_QUADS);

            size_t nTriangles = triangles_.size();

            int nRenderedWingedEdges = 0;

            for(size_t i=0; i<nTriangles; ++i) {
                Triangle const& triangle = triangles_[i];

                if(triangle.isBackFacing) {
                    continue;
                }

                for(size_t j=0; j<3; ++j) { // for each edge
                    size_t adjacent = triangle.adjacent[j];

                    if(adjacent == -1
                        || triangles_[adjacent].isBackFacing) {
                        if(adjacent == -1) {
                            ++nRenderedWingedEdges;
                        }

                        vec3f const& a = vertices_[triangle.indices[j]];
                        vec3f const& b = vertices_[triangle.indices[(j+1)%3]];

                        vec3f al = a - lightPosition;
                        vec3f bl = b - lightPosition;

                        glVertex3fv(b.getRaw());
                        glVertex3fv(a.getRaw());

                        glVertex4f(al.x, al.y, al.z, 0);
                        glVertex4f(bl.x, bl.y, bl.z, 0);
                    }
                }
            }

            glEnd();

            if(nRenderedWingedEdges != nWingedEdges_ / 2) {
                log.warn() 
                    << "Unexpected number of winged edges rendered"
                    << " (expected " << (nWingedEdges_/2)
                    << ", got " << nRenderedWingedEdges << "). "
                    << "Possible reason: lack of numerical precision.";
            }

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();

            float matrix[16] = {
                1,0,0,0,
                0,1,0,0,
                0,0,1,0,
                0,0,0,0
            };

            glMultMatrixf(matrix);

            glTranslatef(-lightPosition.x, -lightPosition.y, -lightPosition.z);

            glBegin(GL_TRIANGLES);

            for(size_t i=0; i<nTriangles; ++i) {
                Triangle const& triangle = triangles_[i];

                if(triangle.isBackFacing) {
                    for(int j=0; j<3; ++j) {
                        vec3f const& v = vertices_[triangle.indices[j]];
                        glVertex3fv(v.getRaw());
                    }
                }
            }

            glEnd();

            glPopMatrix();

            glBegin(GL_TRIANGLES);

            for(size_t i=0; i<nTriangles; ++i) {
                Triangle const& triangle = triangles_[i];

                if(!triangle.isBackFacing) {
                    for(int j=0; j<3; ++j) {
                        vec3f const& v = vertices_[triangle.indices[j]];
                        glVertex3fv(v.getRaw());
                    }
                }
            }

            glEnd();
        }

        Occluder::Triangle::Triangle(int const indices_[3]) 
            : isBackFacing(false) {
                for(int i=0; i<3; ++i) {
                    indices[i] = indices_[i];
                    adjacent[i] = -1;
                }
        }

        Occluder::Occluder(
            std::vector<math::vec3f> const& vertices,
            std::vector<Triangle> const& triangles)
            : vertices_(vertices), triangles_(triangles), nWingedEdges_(0) {
            size_t nTriangles = triangles_.size();

            for(size_t i=0; i<nTriangles; ++i) {
                for(int j=0; j<3; ++j) {
                    if(triangles_[i].adjacent[j] == -1) {
                        ++nWingedEdges_;
                    }
                }
            }

            if(nWingedEdges_ > 0) {
                log.info() << "Occluder model not closed; creating duplicate triangles.";

                for(size_t i=0; i<nTriangles; ++i) {
                    Occluder::Triangle copy(triangles_[i]);

                    // Imagine the triangle indices are (1,2,3)
                    // and the edges are (4,5,6).

                    // Then clearly the edges of a reversed
                    // triangle (3,2,1) are (5,4,6).

                    std::swap(copy.indices[0], copy.indices[2]);
                    std::swap(copy.adjacent[0], copy.adjacent[1]);

                    for(int j=0; j<3; ++j) {
                        if(copy.adjacent[j] != -1) {
                            copy.adjacent[j] += nTriangles;
                        }
                    }

                    triangles_.push_back(copy);
                }

                nWingedEdges_ *= 2;
            }
        }

        void OccluderBuilder::addVertex(vec3f const& vertex) {
            vertices_.push_back(vertex);
        }

        OccluderBuilder::EdgeIndices::EdgeIndices(int firstIndex, int secondIndex) {
            indices[0] = std::min(firstIndex, secondIndex);
            indices[1] = std::max(firstIndex, secondIndex);
        }

        bool OccluderBuilder::EdgeIndices::operator < (
                OccluderBuilder::EdgeIndices const& rhs) const {
            return indices[0] < rhs.indices[0]
                || (indices[0] == rhs.indices[0]
                    && indices[1] < rhs.indices[1]);
        }

        OccluderBuilder::Edge::Edge(std::pair<int,int> firstTriangle) {
            triangles.push_back(firstTriangle);
        }

        void OccluderBuilder::addTriangle(int const indices[3]) {
            Occluder::Triangle triangle(indices);

            const size_t i = triangles_.size();

            triangles_.push_back(triangle);

            for(size_t j=0; j<3; ++j) {
                EdgeIndices edgeIndices(
                    indices[j], 
                    indices[(j+1)%3]);

                std::map<EdgeIndices, Edge>::iterator it = edges_.find(edgeIndices);

                if(it == edges_.end()) {
                    edges_.insert(std::pair<EdgeIndices, Edge>(
                        edgeIndices, Edge(std::pair<int,int>(i,j))));
                } else {
                    Edge& edge = it->second;

                    edge.triangles.push_back(std::pair<int,int>(i,j));

                    assert(edge.triangles.size() <= 2);
                
                    if(edge.triangles.size() == 2) {
                        std::vector<std::pair<int,int>> const& tri =
                            edge.triangles; // shorthand

                        int trng0 = tri[0].first;
                        int edge0 = tri[0].second;

                        int trng1 = tri[1].first;
                        int edge1 = tri[1].second;

                        triangles_[trng0].adjacent[edge0] = trng1;
                        triangles_[trng1].adjacent[edge1] = trng0;
                    }
                }
            }
        }

        bool OccluderBuilder::isClosed() const {
            int wingedCount = 0;

            for(size_t i=0; i<triangles_.size(); ++i) {
                for(int j=0; j<3; ++j) {
                    if(triangles_[i].adjacent[j] == -1) {
                        ++wingedCount;
                    }
                }
            }

            return 0 == wingedCount;
        }

        Occluder* OccluderBuilder::build() const {
            int wingedCount = 0;

            for(size_t i=0; i<triangles_.size(); ++i) {
                for(int j=0; j<3; ++j) {
                    if(triangles_[i].adjacent[j] == -1) {
                        ++wingedCount;
                    }
                }
            }

            log.info() << "Creating a shadow occluder with "
                << vertices_.size() << " vertices, "
                << triangles_.size() << " triangles and "
                << wingedCount << " winged edges.";

            return new Occluder(vertices_, triangles_);
        }
    }
}