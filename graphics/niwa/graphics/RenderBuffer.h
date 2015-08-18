/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_GRAPHICS_RENDERBUFFER_H
#define NIWA_GRAPHICS_RENDERBUFFER_H

#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace graphics {
        class RenderBuffer {
        public:
            enum Type {
                DEPTH,
            };

        public:
            static RenderBuffer* create(Type type, std::pair<size_t, size_t> dimensions);

            ~RenderBuffer();

            unsigned int getHandle() const;

        public:
            class ScopedBind;

        private:
            RenderBuffer();

        private: // prevent copying
            RenderBuffer(RenderBuffer const&);
            RenderBuffer& operator = (RenderBuffer const&);

        private:
            unsigned int handle_;
        };

        class RenderBuffer::ScopedBind {
        public:
            ScopedBind(RenderBuffer const&);
            ~ScopedBind();

        private:
            unsigned int previousHandle_;
        };
    }
}

#endif