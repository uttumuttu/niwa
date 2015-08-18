/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "ClothBuilder.h"

#include "Cloth.h"

#include "niwa/math/vec2i.h"

#include <limits>

namespace niwa {
    namespace dynamics {
        using math::vec3f;

        ClothBuilder::ClothBuilder(
                float mass, float stretchingStiffness,
                float bendingStiffness, float dampingCoefficient,
                float thickness)
            : mass_(mass), stretchingStiffness_(stretchingStiffness),
              bendingStiffness_(bendingStiffness), dampingCoefficient_(dampingCoefficient),
              thickness_(thickness) {
            // ignored
        }

        /**
         * Each edge of the triangular mesh is associated
         * with a stretching spring. The two corner points
         * of the triangle pair connected by each (winged) edge
         * are associated with a bending spring.
         */
        Cloth* ClothBuilder::buildRectangular(
                math::vec3f const& cornerPosition,
                math::vec3f const sides[2],
                math::vec2i const& dimensions,
                bool cornerConstraints[2][2]) const {
            const int nx = dimensions.x;
            const int ny = dimensions.y;

            const int nParticles = nx * ny;

            std::vector<ClothParticle> particles;

            std::vector<ClothSpring> springs;

            for(int y=0; y<ny; ++y) {
                for(int x=0; x<nx; ++x) {
                    float u = x / (nx - 1.0f);
                    float v = y / (ny - 1.0f);

                    vec3f position = 
                        cornerPosition + sides[0] * u + sides[1] * v;

                    particles.push_back(ClothParticle(
                        mass_ / nParticles, position));
                }
            }

            for(int y=0; y<ny; ++y) {
                for(int x=0; x<nx; ++x) {
                    if(y < ny - 1) {
                        springs.push_back(ClothSpring(
                            particles, y*nx+x, (y+1)*nx+x, 
                            stretchingStiffness_, dampingCoefficient_));

                        if(y > 1 && x < nx - 1) {
                            springs.push_back(ClothSpring(
                                particles, (y-1)*nx+x, (y+1)*nx+(x+1), 
                                bendingStiffness_, dampingCoefficient_));
                        }
                    }

                    if(x < nx - 1) {
                        springs.push_back(ClothSpring(
                            particles, y*nx+x, y*nx+(x+1), 
                            stretchingStiffness_, dampingCoefficient_));

                        if(x > 1 && y < ny - 1) {
                            springs.push_back(ClothSpring(
                                particles, y*nx+(x-1), (y+1)*nx+(x+1), 
                                bendingStiffness_, dampingCoefficient_));
                        }
                    }

                    if(x < nx - 1 && y < ny - 1) {
                        springs.push_back(ClothSpring(
                            particles, y*nx+x, (y+1)*nx+(x+1), 
                            stretchingStiffness_, dampingCoefficient_));

                        springs.push_back(ClothSpring(
                            particles, y*nx+(x+1), (y+1)*nx+x, 
                            bendingStiffness_, dampingCoefficient_));
                    }
                }
            }

            if(cornerConstraints) {
                for(int y=0; y<2; ++y) {
                    for(int x=0; x<2; ++x) {
                        if(cornerConstraints[x][y]) {
                            int ix = x ? nx-1 : 0;
                            int iy = y ? ny-1 : 0;

                            particles[iy*nx+ix].setMass( 
                                std::numeric_limits<float>::infinity());
                        }
                    }
                }
            }

            std::vector<Cloth::Triangle> triangles;

            for(int y=0; y<ny-1; ++y) {
                for(int x=0; x<nx-1; ++x) {
                    int idx0[3] = { y*nx+x, y*nx+(x+1), (y+1)*nx+x };
                    int idx1[3] = { y*nx+(x+1), (y+1)*nx+(x+1), (y+1)*nx+x };

                    triangles.push_back(Cloth::Triangle(idx0));
                    triangles.push_back(Cloth::Triangle(idx1));
                }
            }

            return new Cloth(particles, springs, triangles, thickness_);
        }
    }
}