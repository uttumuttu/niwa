/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_GRAPHICS_OCCLUDER_H
#define NIWA_GRAPHICS_OCCLUDER_H

#include "niwa/math/vec3f.h"

#include <vector>
#include <map>

namespace niwa {
    namespace graphics {
        /**
         * A shadow occluder.
         */
        class Occluder {
        public:
            std::vector<math::vec3f> const& getVertices() const;

            std::vector<math::vec3f>& getVertices();

            /**
             * Precomputes triangle facings for
             * silhouette rendering.
             */
            void updateFacings(math::vec3f const& lightPosition) const;

            /**
             * Renders shadow silhouettes.
             */
            void renderSilhouette(math::vec3f const& lightPosition) const;

        private:
            struct Triangle {
                explicit Triangle(int const indices_[3]);

            private:
                int indices[3];

                int adjacent[3];

                mutable bool isBackFacing;

                friend class Occluder;
                friend class OccluderBuilder;
            };

        private:
            Occluder(
                std::vector<math::vec3f> const& vertices,
                std::vector<Triangle> const& triangles);

        private:
            std::vector<math::vec3f> vertices_;

            std::vector<Triangle> triangles_;

            int nWingedEdges_;

            friend class OccluderBuilder;
        };

        class OccluderBuilder {
        private:
            struct EdgeIndices {
                explicit EdgeIndices(int firstIndex, int secondIndex);

                bool operator < (EdgeIndices const&) const;

                int indices[2];
            };

            struct Edge {
                explicit Edge(std::pair<int,int> firstTriangle);

                std::vector<std::pair<int,int>> triangles;
            };

        public:
            void addVertex(math::vec3f const& vertex);

            void addTriangle(int const indices[3]);

            Occluder* build() const;

        private:
            bool isClosed() const;

        private:
            std::vector<math::vec3f> vertices_;

            std::vector<Occluder::Triangle> triangles_;

            std::map<EdgeIndices, Edge> edges_;
        };
    }
}

#endif