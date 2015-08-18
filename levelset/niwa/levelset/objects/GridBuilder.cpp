/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "GridBuilder.h"
#include "Grid.h"

#include "niwa/autodesk/Model.h"
#include "niwa/autodesk/Object.h"

#include "niwa/math/vec3f.h"

#include "niwa/logging/Logger.h"

#include <limits>
#include <map>
#include <vector>
#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

// Exhaustive initialization means that accurate
// distance is computed for each grid point.
// This can be extremely slow, but may be useful
// for debugging.

// #define EXHAUSTIVE_INITIALIZATION

namespace {
    using namespace niwa::logging;
    using namespace niwa::levelset::objects;

    static Logger log(typeid(GridBuilder));
}

namespace {
    using niwa::math::vec3f;
    using niwa::math::vec3i;

    static inline float sign(float x) {
        return x > 0.0f ? 1.0f : (x < 0.0f ? -1.0f : 0.0f);
    }

    static inline vec3f itof(vec3i const& vector) {
        return vec3f(
            static_cast<float>(vector[0]),
            static_cast<float>(vector.y),
            static_cast<float>(vector.z));
    }

    static inline vec3f elementWiseDivision(vec3f const& lhs, vec3f const& rhs) {
        return vec3f(lhs[0] / rhs[0], lhs.y / rhs.y, lhs.z / rhs.z);
    }

    static inline float min(vec3f const& vector) {
        return std::min(vector[0], std::min(vector.y, vector.z));
    }

    static inline float max(vec3f const& vector) {
        return std::max(vector[0], std::max(vector.y, vector.z));
    }
}

namespace {
    using niwa::geom::aabb;
    using niwa::math::vec3f;

    class IElement {
    public:
        virtual ~IElement() {
            // ignored
        }

        virtual bool isDegenerate() const = 0;

        virtual float signedSquareDistanceTo(niwa::math::vec3f const& point) const = 0;

        virtual aabb getBounds() const = 0;
    };

    class Vertex : public IElement {
    public:
        explicit Vertex(niwa::math::vec3f const& position_) : position(position_) {
            // ignored
        }

        float signedSquareDistanceTo(niwa::math::vec3f const& position_) const {
            niwa::math::vec3f relativePosition = position_ - position;

            float squaredDistance = relativePosition.squareLength();

            return squaredDistance * sign(vec3f::dot(relativePosition, normal));
        }

        bool isDegenerate() const {
            return normal.length() == 0;
        }

        aabb getBounds() const {
            return aabb(position);
        }
    public:
        niwa::math::vec3f position;

        niwa::math::vec3f normal;
    };

    /**
     * An index pair of an edge.
     */
    class EdgeIndices {
    public:
        /**
         * @param indices Vertex indices in the original autodesk model;
         *                no ordering required.
         */
        explicit EdgeIndices(int indices[2]) {
            indices_[0] = std::min(indices[0], indices[1]);
            indices_[1] = std::max(indices[0], indices[1]);
        }

        bool operator < (EdgeIndices const& edge) const {
            return indices_[0] < edge.indices_[0]
                || (indices_[0] == edge.indices_[0] 
                &&  indices_[1]  < edge.indices_[1]);
        }

    private:
        /**
         * Vertex indices in the original autodesk model,
         * sorted in ascending order.
         */
        int indices_[2];
    };

    class Edge : public IElement {
    public:
        explicit Edge(boost::array<niwa::math::vec3f,2> vertices_, niwa::math::vec3f const& normal_)
            : vertices(vertices_), normal(normal_) {
            // ignored
        }

        bool isDegenerate() const {
            return vertices[0] == vertices[1] || normal.length() == 0;
        }

        float signedSquareDistanceTo(vec3f const& position_) const {
            vec3f relativePosition = position_ - vertices[0];

            vec3f direction = vertices[1] - vertices[0];

            float alpha = 
                vec3f::dot(relativePosition, direction)
                / vec3f::dot(direction, direction);

            if(alpha > 0 && alpha < 1) {
                direction.normalize();

                relativePosition -= direction * vec3f::dot(relativePosition, direction);

                return relativePosition.squareLength()
                    * sign(vec3f::dot(relativePosition, normal));
            } else {
                return std::numeric_limits<float>::quiet_NaN();
            }
        }

        aabb getBounds() const {
            aabb bounds(vertices[0]);
            bounds.extendToFit(vertices[1]);
            return bounds;
        }

    public:
        boost::array<vec3f,2> vertices;

        vec3f normal;
    };

    class Triangle : public IElement {
    public:
        explicit Triangle(boost::array<vec3f,3> const& vertices_)
            : vertices(vertices_) {
            normal = vec3f::cross(
                    vertices[1] - vertices[0],
                    vertices[2] - vertices[0]);

            normal.normalize();

            for(int i=0; i<3; ++i) {
                vec3f direction = vertices[(i+1)%3] - vertices[i];

                direction.normalize();

                edgeNormals_[i] = vec3f::cross(direction, normal);
            }
        }

        bool isDegenerate() const {
            return normal.length() == 0;
        }

        float signedSquareDistanceTo(vec3f const& position_) const {
            for(int i=0; i<3; ++i) {
                if(vec3f::dot(position_ - vertices[i], edgeNormals_[i]) >= 0) {
                    return std::numeric_limits<float>::quiet_NaN();
                }
            }

            float distance = vec3f::dot(position_ - vertices[0], normal);

            return distance * distance * sign(distance);
        }

        aabb getBounds() const {
            aabb bounds(vertices[0]);
            bounds.extendToFit(vertices[1]);
            bounds.extendToFit(vertices[2]);
            return bounds;
        }
    public:
        boost::array<vec3f,3> vertices;

        vec3f normal;

    private:
        vec3f edgeNormals_[3];
    };

    /**
     * @param elements Ownerships are passed.
     */
    static void extractElements(
            niwa::autodesk::Model const& model, 
            niwa::geom::aabb const& desiredBounds,
            std::vector<boost::shared_ptr<IElement>>& elements) {
        const aabb modelBounds = model.getBounds();

        float scale = min(
            elementWiseDivision(
                desiredBounds.dimensions(),
                modelBounds.dimensions()));

        std::vector<Vertex> outVertices;
        // With edges, eliminating duplicates is important,
        // so map them by indices.
        std::map<EdgeIndices, Edge> outEdges;
        std::vector<Triangle> outTriangles;

        int vertexOffset = 0;

        for(int idxObject=0; idxObject<model.getObjectCount(); ++idxObject) {
            niwa::autodesk::Object const& object = model.getObjectAt(idxObject);

            for(int idxVertex=0; idxVertex<object.getVertexCount(); ++idxVertex) {
                vec3f position;

                object.getVertexAt(idxVertex, position.getRaw());

                // Transform from model bounds to desired bounds.
                position -= modelBounds.center();
                position *= scale;
                position += desiredBounds.center();

                outVertices.push_back(Vertex(position));
            }

            for(int idxFace=0; idxFace<object.getFaceCount(); ++idxFace) {
                int faces[3];

                object.getFaceAt(idxFace, faces);

                boost::array<vec3f,3> triangleVertices;

                for(int i=0; i<3; ++i) {
                    triangleVertices[i] = 
                        outVertices[vertexOffset + faces[i]].position;
                }

                Triangle const triangle(triangleVertices);

                outTriangles.push_back(triangle);

                for(int i=0; i<3; ++i) {
                    outVertices[vertexOffset + faces[i]].normal += triangle.normal;
                }

                for(int i=0; i<3; ++i) {
                    int indices[2];

                    boost::array<vec3f,2> edgeVertices;

                    for(int j=0; j<2; ++j) {
                        indices[j] = vertexOffset + faces[(i+j)%3];

                        edgeVertices[j] = outVertices[indices[j]].position;
                    }

                    EdgeIndices edgeIndices(indices);

                    std::map<EdgeIndices, Edge>::iterator it = outEdges.find(edgeIndices);

                    if(it != outEdges.end()) {
                        it->second.normal += triangle.normal;
                    } else {
                        outEdges.insert(std::pair<EdgeIndices, Edge>(edgeIndices, Edge(edgeVertices, triangle.normal)));
                    }
                }
            }

            vertexOffset += object.getVertexCount();
        }

        BOOST_FOREACH(Vertex& vertex, outVertices) {
            vertex.normal.normalize();

            boost::shared_ptr<IElement> element(new Vertex(vertex));

            elements.push_back(element);
        }

        for(std::map<EdgeIndices, Edge>::iterator it = outEdges.begin();
                it != outEdges.end(); ++it) {
            Edge& edge = it->second;

            edge.normal.normalize();

            boost::shared_ptr<IElement> element(new Edge(edge));

            elements.push_back(element);
        }

        BOOST_FOREACH(Triangle& triangle, outTriangles) {
            boost::shared_ptr<IElement> element(new Triangle(triangle));

            elements.push_back(element);
        }

        std::vector<boost::shared_ptr<IElement>>::iterator it = elements.begin();
        while(it != elements.end()) {
            it = (*it)->isDegenerate() 
                ? elements.erase(it)
                : ++it;
        }
    }
}

namespace {
    static inline float isnan(float x) {
        volatile float y = x;
        return y != y;
    }

    static inline float updateSignedDistance(float oldDistance, float newDistance) {
        if(isnan(oldDistance)) {
            return newDistance;
        } else if(isnan(newDistance)) {
            return oldDistance;
        } else if(std::abs(oldDistance) < std::abs(newDistance)) {
            return oldDistance;
        } else {
            return newDistance;
        }
    }
}

namespace {
    static float* allocateFloatArray(int size) {
        void* p = _mm_malloc(size * sizeof(float), 16);
        if(!p) {
            throw std::bad_alloc(); // ansi-iso compliant
        }
        return static_cast<float*>(p);
    }
}

namespace niwa {
    namespace levelset {
        namespace objects {
            using geom::aabb;

            using math::vec3f;
            using math::vec3i;

            GridBuilder::GridBuilder(
                geom::aabb const& desiredBounds,
                math::vec3i const& dimensions)
                : desiredBounds_(desiredBounds),
                  dimensions_(dimensions) {
                // ignored
            }

            /**
             * @todo This function gives erroneous results on difficult
             *       meshes.
             */
            Grid* GridBuilder::build(autodesk::Model const& model) const {
                std::vector<boost::shared_ptr<IElement>> elements;

                extractElements(model, desiredBounds_, elements);

                // The bounds for the object are
                // extended by one voxel on each side
                // of the cuboid. This ensures that the
                // surface boundaries are captured.

                const vec3i extendedDimensions = dimensions_ + vec3i(2,2,2);

                int nVoxels = extendedDimensions[0] 
                    * extendedDimensions.y 
                    * extendedDimensions.z;

                float* values = allocateFloatArray(nVoxels);

                Grid* grid;
                {
                    aabb extendedBounds(desiredBounds_);

                    vec3f voxelDimensions = elementWiseDivision(
                        desiredBounds_.dimensions(), itof(dimensions_));

                    extendedBounds.extendDimensionsBy(voxelDimensions);

                    grid = new Grid(extendedBounds, 
                        extendedDimensions, values, true);
                }

                for(int i=0; i<nVoxels; ++i) {
                    values[i] = std::numeric_limits<float>::quiet_NaN();
                }

#ifndef EXHAUSTIVE_INITIALIZATION
                BOOST_FOREACH(boost::shared_ptr<IElement> const& element, elements) {
                    aabb elementBounds = element->getBounds();

                    vec3f minf = grid->toLocalCoordinates(
                        elementBounds.minPosition());

                    vec3f maxf = grid->toLocalCoordinates(
                        elementBounds.maxPosition());

                    vec3i mini, maxi;

                    for(int i=0; i<3; ++i) {
                        mini[i] = std::max(
                            0, static_cast<int>(std::floor(minf[i]) - 1));

                        maxi[i] = std::min(
                            extendedDimensions[i] - 1,
                            static_cast<int>(std::ceil(maxf[i]) + 1));
                    }

                    for(int z=mini.z; z<=maxi.z; ++z)
                    for(int y=mini.y; y<=maxi.y; ++y)
                    for(int x=mini[0]; x<=maxi[0]; ++x) {
                        int gridIndex = grid->elementIndex(vec3i(x,y,z));

                        vec3f point(
                            grid->toWorldCoordinates(vec3i(x,y,z)));

                        values[gridIndex] = updateSignedDistance(
                            values[gridIndex], element->signedSquareDistanceTo(point));
                    }
                }
#else
                for(int z=0; z<extendedDimensions.z; ++z) {
                    for(int y=0; y<extendedDimensions.y; ++y) {
                        for(int x=0; x<extendedDimensions[0]; ++x) {
                            int gridIndex = grid->gridIndex(vec3i(x,y,z));

                            vec3f point(
                                grid->toWorldCoordinates(vec3i(x,y,z)));

                            BOOST_FOREACH(boost::shared_ptr<IElement> const& element, elements) {
                                values[gridIndex] = updateSignedDistance(
                                    values[gridIndex], element->signedSquareDistanceTo(point));
                            }
                        }
                    }
                }
#endif

                for(int i=0; i<nVoxels; ++i) {
                    if(!isnan(values[i])) {
                        values[i] = 
                            std::sqrt(
                                std::abs(values[i]))
                            * sign(values[i]);
                    }
                }

                grid->reinitialize();

                return grid;
            }

            Grid* GridBuilder::build(ILevelSet const& levelSet) const {
                int nValues = dimensions_[0] * dimensions_.y * dimensions_.z;

                float* values = allocateFloatArray(nValues);

                Grid* grid = new Grid(desiredBounds_, dimensions_, values,
                    levelSet.isDistanceField());

                for(int z=0; z<dimensions_.z; ++z) {
                    for(int y=0; y<dimensions_.y; ++y) {
                        for(int x=0; x<dimensions_[0]; ++x) {
                            grid->elementAt(x,y,z)
                                = levelSet.value(
                                grid->toWorldCoordinates(
                                vec3i(x,y,z)));
                        }
                    }
                }

                return grid;
            }

            Grid* GridBuilder::buildEmpty() const {
                int nValues = dimensions_[0] * dimensions_.y * dimensions_.z;

                float* values = allocateFloatArray(nValues);

                Grid* grid = new Grid(desiredBounds_, dimensions_, values, false);

                for(int i=0; i<nValues; ++i) {
                    values[i] = std::numeric_limits<float>::infinity();
                }

                return grid;
            }
        }
    }
}