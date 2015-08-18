/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_AUTODESK_IMPORTER_H
#define NIWA_AUTODESK_IMPORTER_H

#include <memory>
#include <string>

namespace niwa {
    namespace autodesk {
        class Model;

        class Importer {
        public:
            Importer();
            ~Importer();

            /**
             * @return The imported model, or null on error.
             */
            Model* importModel(std::string filename) const;

        private:
            Model* readChunks(std::istream& stream) const;
        };
    }
}

#endif