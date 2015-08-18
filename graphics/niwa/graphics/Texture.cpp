/**
* @file
* @author Mikko Kuppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "Texture.h"

#include "Image.h"

#include "niwa/logging/Logger.h"

#include <gl/glew.h>

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

namespace {
    static niwa::logging::Logger Log(typeid(niwa::graphics::Texture));
}

namespace {
    using niwa::graphics::Texture;

    static void setTextureParameters(Texture::Filter filter, Texture::Wrap wrap) {
        switch(filter) {
            case Texture::MIPMAP:
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
                break;
            case Texture::LINEAR:
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                break;
            case Texture::NEAREST:
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                break;
            default:
                Log.warn() << "unknown filter parameter (" << filter << ")";
                break;
        }

        switch(wrap) {
            case Texture::CLAMP:
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                break;
            case Texture::REPEAT:
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                break;
            default:
                Log.warn() << "unknown wrap parameter (" << wrap << ")";
                break;
        }
    }

    static bool getInternalFormat(Texture::Format format, GLuint& internalformat) {
        switch(format) {
        case Texture::RGBA8:
            internalformat = GL_RGBA8;
            return true;
        case Texture::RGBA16F:
            internalformat = GL_RGBA16F;
            return true;
        default:
            return false;
        }
    }
}

namespace niwa {
    namespace graphics {
        Texture* Texture::fromImage(
                boost::shared_ptr<Image> image,
                Texture::Format format,
                Texture::Filter filter, 
                Texture::Wrap wrap) {
            Texture* self = new Texture();

            ScopedBind bind(*self);

            setTextureParameters(filter, wrap);

            GLuint internalformat;
            GLuint sourceformat;

            if(!getInternalFormat(format, internalformat)) {
                Log.warn() << "unknown format parameter (" << format << "), using no image";
                return self;
            }

            if(image) {
                switch(image->getFormat()) {
                case Image::RGB:
                    sourceformat = GL_RGB;
                    break;
                case Image::RGBA:
                    sourceformat = GL_RGBA;
                    break;
                default:
                    Log.warn() << "unknown source format (" << image->getFormat() << "), using no image";
                    return self;
                }

                glTexImage2D(GL_TEXTURE_2D, 0, internalformat, image->getWidth(), image->getHeight(), 0,
                    sourceformat, GL_UNSIGNED_BYTE, image->getData().get());
            } else {
                Log.debug() << "NULL image, returning an empty texture";
            }

            return self;
        }

        Texture* Texture::createEmpty(
                std::pair<size_t, size_t> dimensions,
                Texture::Format format, 
                Texture::Filter filter, Texture::Wrap wrap) {
            Texture* self = new Texture();

            ScopedBind bind(*self);

            setTextureParameters(filter, wrap);

            int width = dimensions.first;
            int height = dimensions.second;

            GLuint internalformat;

            if(!getInternalFormat(format, internalformat)) {
                Log.warn() << "unknown format parameter (" << format << "), using RGBA8";
                internalformat = GL_RGBA8;
            }

            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, 0);  

            return self;
        }

        Texture::~Texture() {
            if(handle_ > 0) {
                glDeleteTextures(1, &handle_);
            }
        }

        Texture::Texture() : handle_(0) {
            glGenTextures(1, &handle_);
        }

        Texture::ScopedBind::ScopedBind(Texture const& texture)
            : previousActive_(0), previousHandle_(0) {
            init(texture, 0);
        }

        Texture::ScopedBind::ScopedBind(Texture const& texture, unsigned int unit)
            : previousActive_(0), previousHandle_(0) {
            init(texture, unit);
        }

        void Texture::ScopedBind::init(Texture const& texture, unsigned int unit) {
            GLint temp;

            glGetIntegerv(GL_ACTIVE_TEXTURE, &temp);
            previousActive_ = temp;

            glGetIntegerv(GL_TEXTURE_BINDING_2D, &temp);
            previousHandle_ = temp;

            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, texture.handle_);
        }

        Texture::ScopedBind::~ScopedBind() {
            glBindTexture(GL_TEXTURE_2D, previousHandle_);
            glActiveTexture(previousActive_);
        }

        unsigned int Texture::getHandle() const {
            return handle_;
        }
    }
}