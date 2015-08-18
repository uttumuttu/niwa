/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "ClothRenderer.h"

#include "niwa/dynamics/Cloth.h"

#include <boost/foreach.hpp>
#include <boost/shared_array.hpp>

#include "niwa/math/fastmath.h"

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

namespace niwa {
    namespace dynamics {
        namespace rendering {
            using graphics::TriangleStrip;
            using graphics::TriangleStripBuilder;

            using math::vec3f;

            ClothRenderer::ClothRenderer(
                    boost::shared_ptr<Cloth> cloth)
                : cloth_(cloth) {
                size_t nParticles = cloth_->getParticles().size();

                for(size_t i=0; i<nParticles; ++i) {
                    vertexNormals_.push_back(vec3f(0,0,0));
                }

                TriangleStripBuilder builder;

                std::vector<Cloth::Triangle> const& triangles =
                    cloth_->getTriangles();

                for(size_t i=0; i<triangles.size(); ++i) {
                    builder.addTriangle(
                        triangles[i].indices);
                }

                builder.getStrips(strips_);
            }

            void ClothRenderer::precomputeFrame() const {
                std::vector<ClothParticle> const& particles =
                    cloth_->getParticles();

                std::vector<Cloth::Triangle> const& triangles = 
                    cloth_->getTriangles();

                for(size_t i=0; i<particles.size(); ++i) {
                    vertexNormals_[i] = vec3f(0,0,0);
                }

                BOOST_FOREACH(Cloth::Triangle const& triangle, triangles) {
                    ClothParticle const& p0 = particles[triangle.indices[0]];
                    ClothParticle const& p1 = particles[triangle.indices[1]];
                    ClothParticle const& p2 = particles[triangle.indices[2]];

                    vec3f normal = vec3f::cross(
                        p1.getPosition() - p0.getPosition(),
                        p2.getPosition() - p0.getPosition());

                    // face normal not normalized for speed

                    for(int i=0; i<3; ++i) {
                        vertexNormals_[triangle.indices[i]] += normal;
                    }
                }

                for(size_t i=0; i<particles.size(); ++i) {
                    vertexNormals_[i] *= niwa::math::rsqrt_nr(
                        vertexNormals_[i].squareLength());
                }
            }

            void ClothRenderer::render() const {
                std::vector<ClothParticle> const& particles =
                    cloth_->getParticles();

                for(size_t i=0; i<strips_.size(); ++i) {
                    std::vector<TriangleStrip::Node> const& nodes = 
                        strips_[i].getNodes();

                    glBegin(GL_TRIANGLE_STRIP);

                    for(size_t j=0; j<nodes.size(); ++j) {
                        int k = nodes[j].vertex;

                        glNormal3fv(vertexNormals_[k].getRaw());
                        glVertex3fv(particles[k].getPosition().getRaw());
                    }

                    glEnd();
                }
            }
        }
    }
}