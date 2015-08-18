/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_AUTODESK_OBJECT_H
#define NIWA_AUTODESK_OBJECT_H

#include "niwa/geom/aabb.h"

#include <string>

namespace niwa {
    namespace autodesk {
        class Object {
        public:
            std::string const& getName() const;

            int getVertexCount() const;
            int getFaceCount() const;

            /**
             * @param index Zero-based vertex index.
             * @param out An array of (at least) three elements.
             */
            void getVertexAt(int index, float* out) const;

            /**
            * @param index Zero-based face index.
            * @param out An array of (at least) three elements.
            */
            void getFaceAt(int index, int* out) const;

            /**
            * Bounding box computation (may take linear time).
            *
            * @return The bounds containing all objects in this model.
            */
            geom::aabb getBounds() const;

            ~Object();

        private:
            Object(std::string const& name);

        private: // prevent copying
            Object(Object const&);
            Object& operator = (Object const&);

        private:
            std::string name_;

            int nVertices_;
            int nFaces_;

            float* vertices_; // owned
            unsigned short* faces_; // owned

            friend class Importer;
        };
    }
}

#endif