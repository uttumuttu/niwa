/**
 * @file
 * @author Mikko Kauppila
 *
 * Based on Paul Bourke's source code.
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "MarchingCubes.h"

#include <cmath>

#include "niwa/math/vec3f.h"
#include "niwa/math/vec3i.h"
#include "niwa/math/FastMath.h"

#include "niwa/levelset/objects/Grid.h"

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

#include <cassert>

#include <xmmintrin.h>

/**
 * Whether to use pointer arithmetic to access
 * the underlying grids.
 */
#define USE_POINTER_ARITHMETIC 1

/**
 * Vertex normals computation causes roughly 20--30%
 * reduction in speed.
 */
#define USE_VERTEX_NORMALS 1

/**
 * Normally the target isolevel is always zero.
 * However, it can be useful to change it temporarily
 * for debugging.
 */
#define ISOLEVEL 0

#include "MarchingCubesTables.inl"

namespace {
    __declspec(align(16)) static const int Pow2[12] = {
        1,2,4,8,16,32,64,128,256,512,1024,2048
    };

    static const int VertexFirst[12] = {
        0,1,2,3,4,5,6,7,0,1,2,3
    };

    static const int VertexSecond[12] = {
        1,2,3,0,5,6,7,4,4,5,6,7
    };
}

namespace {
    using niwa::math::vec3f;
    using niwa::math::vec3i;

    using niwa::levelset::objects::Grid;

    struct Triangle {
        vec3f* positions[3];
        vec3f* normals[3];
    };

    __declspec(align(16)) struct GridCell {
        vec3f points[8];
        float values[8];

        /** Do not set manually; use computeNormals function. */
        vec3f normals[8];

        /** 
         * Computes normals from the grid and set values.
         *
         * @param BOUNDARY Whether all coordinates are greater than zero
         *                 and less than respective grid dimension minus two.
         *                 (Ideally we would have three distinct boundary parameters
         *                 for each coordinate, but the savings would be miniscule.)
         */
        template <bool BOUNDARY>
        __forceinline void computeNormals(Grid const& grid, vec3i const& pos);

        template <> __forceinline void computeNormals<true>(Grid const& grid, vec3i const& pos) {
            assert(pos[0] == 0 || pos[0] == grid.getDimensions()[0]-2);
            assert(pos[1] == 0 || pos[1] == grid.getDimensions()[1]-2);
            assert(pos[2] == 0 || pos[2] == grid.getDimensions()[2]-2);

            computeGradients<0, true>(grid, pos);
            computeGradients<1, true>(grid, pos);
            computeGradients<2, true>(grid, pos);
        }

        template <> __forceinline void computeNormals<false>(Grid const& grid, vec3i const& pos) {
            assert(pos[0] > 0 && pos[0] < grid.getDimensions()[0]-2);
            assert(pos[1] > 0 && pos[1] < grid.getDimensions()[1]-2);
            assert(pos[2] > 0 && pos[2] < grid.getDimensions()[2]-2);

            computeGradients<0, false>(grid, pos);
            computeGradients<1, false>(grid, pos);
            computeGradients<2, false>(grid, pos);
        }

    private:
        /** @return The cell index given relative grid position X,Y,Z (each in {0,1}). */
        template <int X, int Y, int Z> static __forceinline int CELL_IDX();

        template <> static __forceinline int CELL_IDX<0,0,0>() { return 0; }
        template <> static __forceinline int CELL_IDX<0,1,0>() { return 1; }
        template <> static __forceinline int CELL_IDX<1,1,0>() { return 2; }
        template <> static __forceinline int CELL_IDX<1,0,0>() { return 3; }
        template <> static __forceinline int CELL_IDX<0,0,1>() { return 4; }
        template <> static __forceinline int CELL_IDX<0,1,1>() { return 5; }
        template <> static __forceinline int CELL_IDX<1,1,1>() { return 6; }
        template <> static __forceinline int CELL_IDX<1,0,1>() { return 7; }

        /** Computes unscaled gradients along dimension DIM. */
        template <int DIM, bool BOUNDARY>
        __forceinline void computeGradients(Grid const& grid, vec3i const& pos) {
            normals[CELL_IDX<0,0,0>()][DIM] = Grad<DIM,0,0,0,BOUNDARY>::invoke(values, grid, pos);
            normals[CELL_IDX<1,0,0>()][DIM] = Grad<DIM,1,0,0,BOUNDARY>::invoke(values, grid, pos);
            normals[CELL_IDX<0,1,0>()][DIM] = Grad<DIM,0,1,0,BOUNDARY>::invoke(values, grid, pos);
            normals[CELL_IDX<1,1,0>()][DIM] = Grad<DIM,1,1,0,BOUNDARY>::invoke(values, grid, pos);
            normals[CELL_IDX<0,0,1>()][DIM] = Grad<DIM,0,0,1,BOUNDARY>::invoke(values, grid, pos);
            normals[CELL_IDX<1,0,1>()][DIM] = Grad<DIM,1,0,1,BOUNDARY>::invoke(values, grid, pos);
            normals[CELL_IDX<0,1,1>()][DIM] = Grad<DIM,0,1,1,BOUNDARY>::invoke(values, grid, pos);
            normals[CELL_IDX<1,1,1>()][DIM] = Grad<DIM,1,1,1,BOUNDARY>::invoke(values, grid, pos);
        }

        template <int DIM, int DX, int DY, int DZ, bool BOUNDARY> struct Grad { 
            static __forceinline void invoke(float const* values, Grid const& grid, vec3i const& pos); };

        template <int DX, int DY, int DZ, bool B> struct Grad<0,DX,DY,DZ,B> {
            static __forceinline float invoke(float const* values, Grid const& grid, vec3i const& pos) {
                return At<DX+1,DY,DZ,B>::invoke(values,grid,pos) - At<DX-1,DY,DZ,B>::invoke(values,grid,pos); } };

        template <int DX, int DY, int DZ, bool B> struct Grad<1,DX,DY,DZ,B> {
            static __forceinline float invoke(float const* values, Grid const& grid, vec3i const& pos) {
                return At<DX,DY+1,DZ,B>::invoke(values,grid,pos) - At<DX,DY-1,DZ,B>::invoke(values,grid,pos); } };

        template <int DX, int DY, int DZ, bool B> struct Grad<2,DX,DY,DZ,B> {
            static __forceinline float invoke(float const* values, Grid const& grid, vec3i const& pos) {
                return At<DX,DY,DZ+1,B>::invoke(values,grid,pos) - At<DX,DY,DZ-1,B>::invoke(values,grid,pos); } };

        template <int DX, int DY, int DZ, bool B> struct At {
            static __forceinline float invoke(float const* values, Grid const&, vec3i const&) {
                return values[CELL_IDX<DX,DY,DZ>()]; } };

        template <int DY, int DZ, bool B> struct At<-1,DY,DZ,B> {
            static __forceinline float invoke(float const*, Grid const& grid, vec3i const& basePos) {
                return at<B>(grid, basePos + vec3i(-1,DY,DZ)); } };

        template <int DY, int DZ, bool B> struct At<2,DY,DZ,B> {
            static __forceinline float invoke(float const*, Grid const& grid, vec3i const& basePos) {
                return at<B>(grid, basePos + vec3i(2,DY,DZ)); } };

        template <int DX, int DZ, bool B> struct At<DX,-1,DZ,B> {
            static __forceinline float invoke(float const*, Grid const& grid, vec3i const& basePos) {
                return at<B>(grid, basePos + vec3i(DX,-1,DZ)); } };

        template <int DX, int DZ, bool B> struct At<DX,2,DZ,B> {
            static __forceinline float invoke(float const*, Grid const& grid, vec3i const& basePos) {
                return at<B>(grid, basePos + vec3i(DX,2,DZ)); } };

        template <int DX, int DY, bool B> struct At<DX,DY,-1,B> {
            static __forceinline float invoke(float const*, Grid const& grid, vec3i const& basePos) {
                return at<B>(grid, basePos + vec3i(DX,DY,-1)); } };

        template <int DX, int DY, bool B> struct At<DX,DY,2,B> {
            static __forceinline float invoke(float const*, Grid const& grid, vec3i const& basePos) {
                return at<B>(grid, basePos + vec3i(DX,DY,2)); } };

        template <bool B> static __forceinline float at(Grid const&, vec3i const& pos);

        template <> static __forceinline float at<false>(Grid const& grid, vec3i const& pos) {
            return grid.elementAt(pos);
        }

        template <> static __forceinline float at<true>(Grid const& grid, vec3i const& pos) {
            vec3i const& dim = grid.getDimensions();
            vec3i clamp = pos;
            for(int i=0; i<3; ++i) {
                if(clamp[i] < 0) {
                    clamp[i] = 0;
                } else if(clamp[i] >= dim[i]) {
                    clamp[i] = dim[i]-1;
                }
            }
            return grid.elementAt(clamp);
        }
    };
}

#define INTERPOLATION_EPSILON_SQ 0.000000001

namespace {
    using niwa::math::vec3f;
    using niwa::math::vec3i;

    using niwa::levelset::objects::Grid;

    /**
     * Linearly interpolate the position where an isosurface cuts
     * an edge between two vertices, each with their own scalar value.
     */
    static __forceinline void __fastcall lerp(vec3f& result,
            vec3f const& p1, vec3f const& p2, float mu) {
        result.x = p1.x + (p2.x - p1.x) * mu;
        result.y = p1.y + (p2.y - p1.y) * mu;
        result.z = p1.z + (p2.z - p1.z) * mu;
    }

    /**
     * Given a grid cell and an isolevel, calculate the triangular
     * facets required to represent the isosurface through the cell.
     *
     * @param vertices A preallocated array of (at least) 12 vertices.
     *
     * @return The number of triangular facets in [0,5]. The array "triangles"
     *         will be loaded up with the (at most five) vertices.
     *         Zero will be returned if the grid cell is either totally above
     *         of totally below the isolevel.
    */
    static int __fastcall polygonize(
        int const cubeIndex,
        GridCell const& cell, 
        Triangle* triangles,
        vec3f* vertices, vec3f* normals, const bool useVertexNormals) {
        const int edgeValue = EdgeTable[cubeIndex];

        // Find the vertices where the surface intersects the cube

        // Unrolling this loop doesn't help performance.
        for(int i=0; i<12; ++i) {
            if(edgeValue & Pow2[i]) {
                const float valp1 = cell.values[VertexFirst[i]];
                const float valp2 = cell.values[VertexSecond[i]];

                const float vald = valp1 - valp2;

                const float mu = vald * vald > INTERPOLATION_EPSILON_SQ
                    ? (valp1 - ISOLEVEL) / vald : 0.5f;

                lerp(vertices[i],
                    cell.points[VertexFirst[i]], cell.points[VertexSecond[i]], mu);

                if(useVertexNormals) {
                    lerp(normals[i],
                        cell.normals[VertexFirst[i]], cell.normals[VertexSecond[i]], mu);
                }
            }
        }

        // Create the triangles
        int nTriangles = 0;

        int const*const triArray = TriTable[cubeIndex];

        // It is true that triArray[0] != -1 at this point.
        // Unfortunately, a for loop is still the fastest.
        for(int i=0; triArray[i] != -1; i+=3, ++nTriangles) {
            triangles[nTriangles].positions[0] = &vertices[triArray[i]];
            triangles[nTriangles].positions[1] = &vertices[triArray[i+1]];
            triangles[nTriangles].positions[2] = &vertices[triArray[i+2]];

            if(useVertexNormals) {
                triangles[nTriangles].normals[0] = &normals[triArray[i]];
                triangles[nTriangles].normals[1] = &normals[triArray[i+1]];
                triangles[nTriangles].normals[2] = &normals[triArray[i+2]];
            }
        }

        assert(nTriangles <= 5);

        return nTriangles;
    }
}

namespace niwa {
    namespace graphics {
        using geom::aabb;
        using math::vec3i;

        MarchingCubes::MarchingCubes() : useOpenGl_(true), useVertexNormals_(true) {
            // ignored
        }

        void MarchingCubes::setUseOpenGl(bool useOpenGl) {
            useOpenGl_ = useOpenGl;
        }

        bool MarchingCubes::getUseOpenGl() const {
            return useOpenGl_;
        }

        void MarchingCubes::setUseVertexNormals(bool value) {
            useVertexNormals_ = value;
        }

        bool MarchingCubes::getUseVertexNormals() const {
            return useVertexNormals_;
        }

        void MarchingCubes::render(
                levelset::objects::Grid const& grid) const {
            __declspec(align(16)) const aabb bounds = grid.getBounds();

            __declspec(align(16)) const vec3i dimensions = grid.getDimensions();

            if(dimensions.x <= 1 || dimensions.y <= 1 || dimensions.z <= 1) {
                // Many indexing operations (e.g., vertex normal computation)
                // aren't safe for degenerate grids.
                return;
            }

            GridCell cell;

            Triangle triangles[5];

            __declspec(align(16)) vec3f vertices[12];
            __declspec(align(16)) vec3f normals[12];

            __declspec(align(16)) const vec3f h(
                bounds.dimensions()[0] / dimensions[0],
                bounds.dimensions()[1] / dimensions[1],
                bounds.dimensions()[2] / dimensions[2]);

            __declspec(align(16)) const vec3f hInv(1/h.x, 1/h.y, 1/h.z);

            __declspec(align(16)) const vec3f firstVoxelPosition = 
                bounds.minPosition() + h * .5f;

            __declspec(align(16)) vec3f cellBasePosition = firstVoxelPosition;

            if(useOpenGl_) {
                glEnable(GL_CULL_FACE);
                glEnable(GL_DEPTH_TEST);

                glBegin(GL_TRIANGLES);
            }

            cell.points[0] = vec3f(0,0,0);
            cell.points[1] = vec3f(0,h.y,0);
            cell.points[2] = vec3f(h.x,h.y,0);
            cell.points[3] = vec3f(h.x,0,0);
            cell.points[4] = vec3f(0,0,h.z);
            cell.points[5] = vec3f(0,h.y,h.z);
            cell.points[6] = vec3f(h.x,h.y,h.z);
            cell.points[7] = vec3f(h.x,0,h.z);

#if USE_POINTER_ARITHMETIC
            float const* ptr_z = grid.getRaw();
#endif

            for(int z=0; z<dimensions.z-1; ++z, cellBasePosition.z += h.z) {
                cellBasePosition.y = firstVoxelPosition.y;

#if USE_POINTER_ARITHMETIC
                float const* ptr_y = ptr_z;
                ptr_z += dimensions.x * dimensions.y; 
#endif

                for(int y=0; y<dimensions.y-1; ++y, cellBasePosition.y += h.y) {
                    cellBasePosition.x = firstVoxelPosition.x;

                    cell.values[2] = grid.elementAt(0,y+1,z);
                    cell.values[3] = grid.elementAt(0,y,z);
                    cell.values[6] = grid.elementAt(0,y+1,z+1);
                    cell.values[7] = grid.elementAt(0,y,z+1);

                    int cubeIndex = 0;

                    if(cell.values[2] < ISOLEVEL) { cubeIndex |= 4; }
                    if(cell.values[3] < ISOLEVEL) { cubeIndex |= 8; }
                    if(cell.values[6] < ISOLEVEL) { cubeIndex |= 64; }
                    if(cell.values[7] < ISOLEVEL) { cubeIndex |= 128; }

#if USE_POINTER_ARITHMETIC
                    float const* ptr_x = ptr_y + 1;
                    ptr_y += dimensions.y; 
#endif

                    for(int x=0; x<dimensions.x-1; ++x, cellBasePosition.x += h.x, ++ptr_x) {
                        cubeIndex = 
                            ((cubeIndex&136)  >> 3) |
                            ((cubeIndex&68)   >> 1);

                        cell.values[0] = cell.values[3];
                        cell.values[1] = cell.values[2];
                        cell.values[4] = cell.values[7];
                        cell.values[5] = cell.values[6];

#if USE_POINTER_ARITHMETIC
                        cell.values[2] = ptr_x[dimensions.x];
                        cell.values[3] = ptr_x[0];
                        cell.values[6] = ptr_x[dimensions.x + dimensions.x*dimensions.y];
                        cell.values[7] = ptr_x[dimensions.x*dimensions.y];
#else
                        cell.values[2] = grid.elementAt(x+1,y+1,z);
                        cell.values[3] = grid.elementAt(x+1,y,z);
                        cell.values[6] = grid.elementAt(x+1,y+1,z+1);
                        cell.values[7] = grid.elementAt(x+1,y,z+1);
#endif

                        if(cell.values[2] < ISOLEVEL) { cubeIndex |= 4; }
                        if(cell.values[3] < ISOLEVEL) { cubeIndex |= 8; }
                        if(cell.values[6] < ISOLEVEL) { cubeIndex |= 64; }
                        if(cell.values[7] < ISOLEVEL) { cubeIndex |= 128; }

                        if(!EdgeTable[cubeIndex]) {
                            // Cube is entirely in/out of the surface.
                            // (This is the common case, so it's a good idea
                            // to bail. Also, bailing is necessary for future
                            // code to work.)
                            continue;
                        }

                        if(useVertexNormals_) {
                            if(z==0 || z==dimensions.z-2 
                                || y==0 || y==dimensions.y-2
                                || x==0 || x==dimensions.x-2) {
                                cell.computeNormals<true>(grid, vec3i(x,y,z));
                            } else {
                                cell.computeNormals<false>(grid, vec3i(x,y,z));
                            }
                        }

                        const int nTriangles = polygonize(
                            cubeIndex, cell, 
                            triangles, vertices, normals,
                            useVertexNormals_);

                        for(int i=0; i<nTriangles; ++i) {
                            __m128 invLengths;

                            if(!useVertexNormals_) {
                                vec3f normal = vec3f::cross(
                                    *triangles[i].positions[1] - *triangles[i].positions[0],
                                    *triangles[i].positions[2] - *triangles[i].positions[0]);

                                normal *= math::rsqrt_nr(normal.squareLength());

                                if(useOpenGl_) {
                                    glColor3fv(normal.getRaw());
                                }
                            } else {
                                for(int j=0; j<3; ++j) {
                                    vec3f& normal = *triangles[i].normals[j];

                                    normal.x *= hInv.x;
                                    normal.y *= hInv.y;
                                    normal.z *= hInv.z;
                                }

                                invLengths = _mm_rsqrt_ps(_mm_setr_ps(
                                        triangles[i].normals[0]->squareLength(),
                                        triangles[i].normals[1]->squareLength(),
                                        triangles[i].normals[2]->squareLength(), 1));
                            }

                            for(int j=0; j<3; ++j) {
                                if(useVertexNormals_) {
                                    vec3f& normal = *triangles[i].normals[j];

                                    normal *= invLengths.m128_f32[j];

                                    if(useOpenGl_) {
                                        glColor3fv(normal.getRaw());
                                    }
                                }

                                const vec3f position = cellBasePosition 
                                    + *triangles[i].positions[j];

                                if(useOpenGl_) {
                                    glVertex3fv(position.getRaw());
                                }
                            }
                        }
                    }
                }
            }

            if(useOpenGl_) {
                glEnd();
            }
        }
    }
}