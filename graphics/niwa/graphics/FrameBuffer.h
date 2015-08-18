/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_GRAPHICS_FRAMEBUFFER_H
#define NIWA_GRAPHICS_FRAMEBUFFER_H

#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace graphics {
        class RenderBuffer;
        class Texture;

        class FrameBuffer {
        public:
            /**
             * Creates a frame buffer with depth capability.
             */
            static FrameBuffer* create(
                boost::shared_ptr<Texture> texture,
                boost::shared_ptr<RenderBuffer> depthBuffer);

            /**
             * Creates a frame buffer without depth capability.
             */
            static FrameBuffer* create(
                boost::shared_ptr<Texture> texture);

            ~FrameBuffer();

            bool isComplete() const;

        public:
            class ScopedBind;

        private:
            FrameBuffer(
                boost::shared_ptr<Texture> texture, 
                boost::shared_ptr<RenderBuffer> depthBuffer);

        private: // prevent copying
            FrameBuffer(FrameBuffer const&);
            FrameBuffer& operator = (FrameBuffer const&);

        private:
            unsigned int handle_;

            bool isComplete_;

            boost::shared_ptr<Texture> texture_;
            boost::shared_ptr<RenderBuffer> depthBuffer_;
        };

        class FrameBuffer::ScopedBind {
        public:
            ScopedBind(FrameBuffer const&);

            ~ScopedBind();

        private:
            unsigned int previousHandle_;
        };
    }
}

#endif