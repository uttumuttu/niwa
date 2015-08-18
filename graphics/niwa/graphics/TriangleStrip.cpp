/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "TriangleStrip.h"

#include "niwa/logging/Logger.h"

#include <boost/shared_array.hpp>

#include <cassert>

namespace niwa {
    namespace graphics {
        static logging::Logger log(typeid(TriangleStrip));

        static int getNewVertex(
                std::vector<TriangleStrip::Node> const& nodes,
                int const triangleVertices[3]) {
            // The last two vertices.
            int v0 = nodes[nodes.size()-2].vertex;
            int v1 = nodes[nodes.size()-1].vertex;

            if((nodes.size()%2) == 1) {
                std::swap(v0, v1);
            }

            for(int i=0; i<3; ++i) {
                if(triangleVertices[i] == v0
                    && triangleVertices[(i+1)%3] == v1) {
                        return triangleVertices[(i+2)%3];
                }
            }

            return -1;
        }

        bool TriangleStripBuilder::EdgeIndices::operator < (
                TriangleStripBuilder::EdgeIndices const& rhs) const {
            return vertices[0] < rhs.vertices[0]
                || (vertices[0] == rhs.vertices[0]
                    && vertices[1] < rhs.vertices[1]);
        }

        std::vector<TriangleStrip::Node> const& TriangleStrip::getNodes() const {
            return nodes_;
        }

        TriangleStrip::TriangleStrip(std::vector<TriangleStrip::Node> const& nodes)
            : nodes_(nodes) {
            // ignored
        }

        void TriangleStripBuilder::addTriangle(int const vertices[3]) {
            size_t idxTriangle = triangles_.size();

            triangles_.push_back(Triangle(vertices));

            for(int i=0; i<3; ++i) {
                EdgeIndices edgeIndices(vertices[i], vertices[(i+1)%3]);

                std::map<EdgeIndices, Edge>::iterator it = edges_.find(edgeIndices);

                if(it == edges_.end()) {
                    edges_.insert(std::pair<EdgeIndices, Edge>(
                        edgeIndices, Edge(std::pair<int,int>(idxTriangle, i))));
                } else {
                    Edge& edge = it->second;

                    edge.triangles.push_back(
                        std::pair<int,int>(idxTriangle, i));

                    assert(edge.triangles.size() <= 2);

                    if(edge.triangles.size() == 2) {
                        int trng0 = edge.triangles[0].first;
                        int trng1 = edge.triangles[1].first;

                        int edge0 = edge.triangles[0].second;
                        int edge1 = edge.triangles[1].second;

                        triangles_[trng0].adjacent[edge0] = trng1;
                        triangles_[trng1].adjacent[edge1] = trng0;
                    }
                }
            }
        }

        void TriangleStripBuilder::getStrips(
                std::vector<TriangleStrip>& out) const {
            const int originalSize = out.size();

            boost::shared_array<bool> isUsed(
                new bool[triangles_.size()]);

            for(size_t i=0; i<triangles_.size(); ++i) {
                isUsed[i] = false;
            }

            for(size_t i=0; i<triangles_.size(); ++i) {
                if(isUsed[i]) {
                    continue;
                } else {
                    isUsed[i] = true;
                }

                Triangle const* triangle = &triangles_[i];

                std::vector<TriangleStrip::Node> nodes;

                for(int j=0; j<3; ++j) {
                    nodes.push_back(TriangleStrip::Node(
                        triangle->vertices[j], i));
                }

                bool neighborFound;

                do {
                    neighborFound = false;

                    for(int j=0; j<3; ++j) {
                        int adjacent = triangle->adjacent[j];

                        if(adjacent == -1 || isUsed[adjacent]) {
                            continue;
                        }

                        Triangle const* neighbor = &triangles_[adjacent];

                        int newVertex = 
                            getNewVertex(nodes, neighbor->vertices);

                        if(newVertex != -1) {
                            isUsed[adjacent] = true;

                            nodes.push_back(TriangleStrip::Node(
                                newVertex, adjacent));

                            triangle = neighbor;

                            neighborFound = true;

                            break;
                        }
                    }
                } while(neighborFound);

                out.push_back(TriangleStrip(nodes));
            }

            log.info() << 
                "Built " << (out.size() - originalSize)
                << " triangle strips from "
                << triangles_.size() << " triangles.";
        }

        TriangleStripBuilder::Triangle::Triangle(int const vertices_[3]) {
            for(int i=0; i<3; ++i) {
                vertices[i] = vertices_[i];

                adjacent[i] = -1;
            }
        }

        TriangleStripBuilder::EdgeIndices::EdgeIndices(int firstVertex, int secondVertex) {
            vertices[0] = std::min(firstVertex, secondVertex);
            vertices[1] = std::max(firstVertex, secondVertex);
        }

        TriangleStripBuilder::Edge::Edge(std::pair<int,int> firstTriangle) {
            triangles.push_back(firstTriangle);
        }

        TriangleStrip::Node::Node(int vertex_, int triangle_) {
            vertex = vertex_;
            triangle = triangle_;
        }
    }
}