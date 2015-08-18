/**
 * @file
 * @author Mikko Kuppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_GRAPHICS_TEXTURE_H
#define NIWA_GRAPHICS_TEXTURE_H

#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace graphics {
        class Image;

        /**
         * A two-dimensional managed OpenGL texture.
         */
        class Texture {
        public:
            /**
             * Internal format of a texture.
             */
            enum Format {
                RGBA8,
                RGBA16F
            };

            enum Filter {
                NEAREST,
                LINEAR,
                MIPMAP
            };

            enum Wrap {
                CLAMP,
                REPEAT
            };

        public:
            /**
             * @param image The source image. If NULL, returns an empty texture.
             */
            static Texture* fromImage(
                boost::shared_ptr<Image> image,
                Format format,
                Filter filter, Wrap wrap);

            static Texture* createEmpty(
                std::pair<size_t, size_t> dimensions,
                Format format,
                Filter filter, Wrap wrap);

            ~Texture();

            unsigned int getHandle() const;

        public:
            class ScopedBind;

        private:
            Texture();

        private: // prevent copying
            Texture(Texture const&);
            Texture& operator = (Texture const&);

        private:
            unsigned int handle_;
        };

        /**
         * A RAII helper class for binding Texture objects.
         */
        class Texture::ScopedBind {
        public:
            /**
             * Binds the texture to the default texture unit (zero).
             */
            ScopedBind(Texture const&);

            /**
             * Binds the texture to the given zero-based texture unit.
             */
            ScopedBind(Texture const&, unsigned int unit);

            ~ScopedBind();

        private:
            void init(Texture const&, unsigned int unit);

        private:
            unsigned int previousActive_;

            unsigned int previousHandle_;
        };
    }
}

#endif