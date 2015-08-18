/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "FrameBuffer.h"

#include "Texture.h"
#include "RenderBuffer.h"

#include <gl/glew.h>

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

namespace niwa {
    namespace graphics {
        FrameBuffer::FrameBuffer(
            boost::shared_ptr<Texture> texture, 
            boost::shared_ptr<RenderBuffer> depthBuffer)
        : handle_(0), isComplete_(false), texture_(texture), depthBuffer_(depthBuffer) {
            glGenFramebuffersEXT(1, &handle_);

            ScopedBind bind(*this);

            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                GL_TEXTURE_2D, texture_->getHandle(), 0);

            if(depthBuffer_) {
                glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                    GL_RENDERBUFFER_EXT, depthBuffer_->getHandle());
            }

            GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

            isComplete_ = status == GL_FRAMEBUFFER_COMPLETE_EXT;
        }

        FrameBuffer::~FrameBuffer() {
            glDeleteFramebuffersEXT(1, &handle_);
        }

        FrameBuffer::ScopedBind::ScopedBind(FrameBuffer const& buffer) : previousHandle_(0) {
            GLint temp;

            glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &temp);

            previousHandle_ = temp;

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, buffer.handle_);
        }

        FrameBuffer::ScopedBind::~ScopedBind() {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, previousHandle_);
        }

        FrameBuffer* FrameBuffer::create(
            boost::shared_ptr<Texture> texture, 
            boost::shared_ptr<RenderBuffer> depthBuffer) {
            return new FrameBuffer(texture, depthBuffer);
        }

        FrameBuffer* FrameBuffer::create(
            boost::shared_ptr<Texture> texture) {
            boost::shared_ptr<RenderBuffer> depthBuffer;

            return new FrameBuffer(texture, depthBuffer);
        }

        bool FrameBuffer::isComplete() const {
            return isComplete_;
        }
    }
}