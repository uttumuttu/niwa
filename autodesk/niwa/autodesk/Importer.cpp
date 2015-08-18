/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/autodesk/Importer.h"

#include "niwa/autodesk/Model.h"
#include "niwa/autodesk/Object.h"

#include "niwa/logging/Logger.h"

#include <fstream>
#include <sstream>

#define CHUNK_MAIN 0x4D4D
#define CHUNK_VERSION 0x0002
#define CHUNK_EDITOR 0x3D3D
#define CHUNK_OBJECT 0x4000
#define CHUNK_TRIANGULAR_MESH 0x4100
#define CHUNK_VERTICES_LIST 0x4110
#define CHUNK_FACES_DESCRIPTION 0x4120

namespace {
    static unsigned short readShort(std::istream& stream) {
        unsigned short out;
        stream.read(reinterpret_cast<char*>(&out), 2);
        return out;
    }

    static unsigned int readInt(std::istream& stream) {
        unsigned int out;
        stream.read(reinterpret_cast<char*>(&out), 4);
        return out;
    }

    static std::string readAsciiZ(std::istream& stream) {
        std::stringstream ss;

        int ch;

        while((ch = stream.get()) != 0) {
            ss << static_cast<char>(ch);
        }

        return ss.str();
    }
}

namespace niwa {
    namespace autodesk {
        static logging::Logger log(typeid(Importer));

        Importer::Importer() {
            // ignored
        }

        Importer::~Importer() {
            // ignored
        }

        Model* Importer::importModel(std::string filename) const {
            log.info() << "importing " << filename;

            std::ifstream stream;

            stream.open(filename.c_str(), std::ios::in | std::ios::binary);

            if(!stream.is_open()) {
                log.error() << "could not open file " << filename;
                return 0;
            } else {
                Model* result = readChunks(stream);
                stream.close();
                return result;
            }
        }

        Model* Importer::readChunks(std::istream& stream) const {
            stream.seekg(0, std::ios::end);
            const int streamLength = stream.tellg();
            stream.seekg(0, std::ios::beg);

            Model* model = new Model();

            Object* object = 0;

            while(stream.tellg() < streamLength) {
                unsigned short chunkId = readShort(stream);
                unsigned int chunkLength = readInt(stream);

                switch(chunkId) {
                case CHUNK_MAIN:
                    log.debug() << "main chunk found";
                    break;
                case CHUNK_EDITOR:
                    log.debug() << "editor chunk found";
                    break;
                case CHUNK_OBJECT:
                    if(object) {
                        model->objects_.push_back(object);
                        object = 0;
                    }
                    object = new Object(readAsciiZ(stream));

                    log.debug() << "object chunk '" << object->getName() << "'found";

                    break;
                case CHUNK_TRIANGULAR_MESH:
                    log.debug() << "mesh chunk found";
                    break;
                case CHUNK_VERTICES_LIST:
                    if(object->vertices_) {
                        delete[] object->vertices_;
                        object->vertices_ = 0;
                    }

                    object->nVertices_ = readShort(stream);

                    log.debug() << "vertex list with " << object->nVertices_ << " vertices found";

                    object->vertices_ = new float[3 * object->nVertices_];

                    stream.read(
                        reinterpret_cast<char*>(object->vertices_),
                        object->nVertices_ * 3 * 4);
                    break;
                case CHUNK_FACES_DESCRIPTION:
                    if(object->faces_) {
                        delete[] object->faces_;
                        object->faces_ = 0;
                    }

                    object->nFaces_ = readShort(stream);

                    log.debug() << "face list with " << object->nFaces_ << " faces found";

                    object->faces_ = new unsigned short[3 * object->nFaces_];

                    for(int i=0; i<object->nFaces_; ++i) {
                        stream.read(
                            reinterpret_cast<char*>(&object->faces_[3*i]),
                            3 * 2);

                        readShort(stream); // skip faces flag
                    }
                    
                    break;
                default:
                    log.debug() << "skipping chunk with id " << chunkId;

                    stream.seekg(chunkLength - 6, std::ios::cur);
                    break;
                }
            }

            if(object) {
                model->objects_.push_back(object);
                object = 0;
            }

            log.debug() << "model loaded";
            return model;
         }
    }
}