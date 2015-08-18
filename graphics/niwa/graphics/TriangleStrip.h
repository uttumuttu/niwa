/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_GRAPHICS_TRIANGLESTRIP_H
#define NIWA_GRAPHICS_TRIANGLESTRIP_H

#include <vector>
#include <map>

namespace niwa {
    namespace graphics {
        class TriangleStrip {
        public:
            struct Node {
                int vertex;

                int triangle;

            private:
                Node(int vertex, int triangle);

                friend class TriangleStripBuilder;
            };

            std::vector<Node> const& getNodes() const;

        private:
            explicit TriangleStrip(std::vector<Node> const& nodes);

        private:
            std::vector<Node> nodes_;

            friend class TriangleStripBuilder;
        };

        class TriangleStripBuilder {
        public:
            void addTriangle(int const vertices[3]);

            /**
             * Gets the current strips.
             */
            void getStrips(std::vector<TriangleStrip>& out) const;

        private:
            struct Triangle {
                explicit Triangle(int const vertices_[3]);

                int vertices[3];

                int adjacent[3];
            };

            struct EdgeIndices {
                EdgeIndices(int firstVertex, int secondVertex);

                bool operator < (EdgeIndices const& rhs) const;

                int vertices[2];
            };

            struct Edge {
                explicit Edge(std::pair<int,int> firstTriangle);

                std::vector<std::pair<int,int>> triangles;
            };

        private:
            std::vector<Triangle> triangles_;

            std::map<EdgeIndices, Edge> edges_;
        };
    }
}

#endif