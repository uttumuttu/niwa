/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "RenderBuffer.h"

#include <gl/glew.h>

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

namespace niwa {
    namespace graphics {
        RenderBuffer* RenderBuffer::create(RenderBuffer::Type type, std::pair<size_t, size_t> dimensions) {
            RenderBuffer* self = new RenderBuffer();

            ScopedBind bind(*self);

            size_t width = dimensions.first;
            size_t height = dimensions.second;

            switch(type) {
                case DEPTH:
                    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
                        width, height);
                    break;
                default:
                    break;
            }

            return self;
        }

        RenderBuffer::RenderBuffer() : handle_(0) {
            glGenRenderbuffersEXT(1, &handle_);
        }

        RenderBuffer::~RenderBuffer() {
            glDeleteRenderbuffersEXT(1, &handle_);
        }

        RenderBuffer::ScopedBind::ScopedBind(RenderBuffer const& buffer) : previousHandle_(0) {
            GLint temp;

            glGetIntegerv(GL_RENDERBUFFER_BINDING_EXT, &temp);

            previousHandle_ = temp;

            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, buffer.handle_);
        }

        RenderBuffer::ScopedBind::~ScopedBind() {
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, previousHandle_);
        }

        unsigned int RenderBuffer::getHandle() const {
            return handle_;
        }
    }
}
