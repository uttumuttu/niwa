/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_GRAPHICS_IMAGE_H
#define NIWA_GRAPHICS_IMAGE_H

#include <boost/shared_array.hpp>

namespace niwa {
    namespace graphics {
        /**
         * A two-dimensional image.
         */
        class Image {
        public:
            enum Format {
                RGB,
                RGBA
            };

        public:
            /**
             * Currently supports only the JPEG format.
             *
             * @return The loaded image or NULL if the image
             *         couldn't be loaded.
             */
            static Image* create(std::string const& filename, Format format);

        public:
            ~Image();

            size_t getWidth() const;
            size_t getHeight() const;

            std::pair<size_t, size_t> getDimensions() const;

            /**
             * @return The data in row-major format.
             */
            boost::shared_array<unsigned char> const getData() const;

            /**
             * @return The data in row-major format.
             */
            boost::shared_array<unsigned char> getData();

            Format getFormat() const;

        private:
            Image(size_t width, size_t height, Format format, boost::shared_array<unsigned char> data);

        private: // prevent copying
            Image(Image const&);
            Image& operator = (Image const&);

        private:
            size_t width_;
            size_t height_;

            Format format_;

            boost::shared_array<unsigned char> data_;
        };
    }
}

#endif