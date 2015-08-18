/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_AUTODESK_MODEL_H
#define NIWA_AUTODESK_MODEL_H

#include "niwa/geom/aabb.h"

#include <vector>

namespace niwa {
    namespace autodesk {
        class Object;

        class Model {
        public:
            ~Model();

            int getObjectCount() const;

            Object const& getObjectAt(int index) const;

            /**
             * Bounding box computation (may take linear time).
             *
             * @return The bounds containing all objects in this model.
             */
            geom::aabb getBounds() const;

        private:
            Model();

        private: // prevent copying
            Model(Model const&);
            Model& operator = (Model const&);

        public:
            std::vector<Object*> objects_; // owned

            friend class Importer;
        };
    }
}

#endif