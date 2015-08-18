/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "Image.h"

#include "jpeglib.h"
#include "jerror.h"

#include <setjmp.h>

#include "niwa/logging/Logger.h"

namespace {
    static niwa::logging::Logger Log(typeid(niwa::graphics::Image));
}

namespace {
    struct JpegErrorManager {
        /**
         * The first field must be the base
         * jpeg_error_mgr object; the latter
         * fields are used for manual extensions.
         */
        struct jpeg_error_mgr pub;

        jmp_buf setjmp_buffer;
    };

    /**
     * @brief Receives control for a fatal error.
     *
     * (Verbatim from libjpeg documentation.)
     *
     * Information sufficient to generate the error message has been 
     * stored in cinfo->err; call output_message to display it.
     * Control must NOT return to the caller; generally this routine
     * will exit() or longjmp() somewhere. Typically you would override
     * this routine to get rid of the exit() default behavior. Note that
     * if you continue processing, you should clean up the JPEG object
     * with jpeg_abort() or jpeg_destroy(). 
     */
    static void jpegErrorExit(j_common_ptr cinfo) {
        // Since interaction between jumping and
        // object destruction is non-portable, it's
        // best to use a distinct scope for the non-jumping portion.
        {
            char buffer[JMSG_LENGTH_MAX];

            (*cinfo->err->format_message) (cinfo, buffer);

            Log.error() << buffer;
        }

        JpegErrorManager* mgr = 
            reinterpret_cast<JpegErrorManager*>(cinfo->err);

        longjmp(mgr->setjmp_buffer, 1);
    }

    /**
     * @brief Decide whether or not to emit a warning or trace message; if so, calls output_message.
     *
     * (Verbatim from libjpeg documentation.)
     *
     * The main reason for overriding this method would be to abort on warnings.
     *
     * @param msg_level -1 for warnings, 0 and up for trace messages
     */
    static void jpegEmitMessage(j_common_ptr cinfo, int msg_level) {
        char buffer[JMSG_LENGTH_MAX];

        (*cinfo->err->format_message) (cinfo, buffer);

        if(msg_level > 0) {
            // Do nothing on debug messages; we assume
            // libjpeg is bug-free.
        } else if(msg_level == 0) {
            Log.info() << buffer;
        } else {
            Log.warn() << buffer;
        }
    }
}

namespace {
    /**
     * Adds a fully opaque alpha channel to RGB source data.
     */
    static boost::shared_array<unsigned char> addAlphaChannel(
        size_t nPixels, boost::shared_array<unsigned char> source) {
        boost::shared_array<unsigned char> result(new unsigned char[4*nPixels]);

        for(size_t i=0; i<nPixels; ++i) {
            for(size_t j=0; j<3; ++j) {
                result[4*i+j] = source[3*i+j];
            }
            result[4*i+3] = 255;
        }
        return result;
    }
}

namespace niwa {
    namespace graphics {
        Image* Image::create(std::string const& filename, Format format) {
            unsigned char* data = 0;

            FILE* infile = 0;

            if(fopen_s(&infile, filename.c_str(), "rb")) {
                return 0;
            }

            struct jpeg_decompress_struct cinfo;

            struct JpegErrorManager jerr;

            cinfo.err = jpeg_std_error(&jerr.pub);

            // register manual callbacks
            jerr.pub.error_exit = jpegErrorExit;
            jerr.pub.emit_message = jpegEmitMessage;

#pragma warning(disable:4611)
            if(setjmp(jerr.setjmp_buffer)) {
                // This is where we jump on errors.
                // Notice that interaction between
                // jumping and C++ object destruction 
                // is non-portable, so we must not
                // use managed objects during image loading.

                delete[] data;
                data = 0;
                jpeg_destroy_decompress(&cinfo);
                fclose(infile);
                return 0;
            }

            jpeg_create_decompress(&cinfo);

            jpeg_stdio_src(&cinfo, infile);
            jpeg_read_header(&cinfo, TRUE);
            jpeg_start_decompress(&cinfo);

            const int w = cinfo.output_width;
            const int h = cinfo.output_height;

            data = new unsigned char[w*h*3];

            for(int i=0; i<h; ++i) {
                unsigned char* ptr = &data[i*w*3];
                jpeg_read_scanlines(&cinfo, &ptr, 1);
            }

            jpeg_finish_decompress(&cinfo);
            jpeg_destroy_decompress(&cinfo);
            fclose(infile);

            boost::shared_array<unsigned char> result(data);

            if(format == RGBA) {
                result = addAlphaChannel(w*h, result);
            }
            return new Image(w, h, format, result);
        }

        Image::~Image() {
            // ignored
        }

        size_t Image::getWidth() const {
            return width_;
        }

        size_t Image::getHeight() const {
            return height_;
        }

        std::pair<size_t, size_t> Image::getDimensions() const {
            return std::pair<size_t, size_t>(width_, height_);
        }

        boost::shared_array<unsigned char> const Image::getData() const {
            return data_;
        }

        boost::shared_array<unsigned char> Image::getData() {
            return data_;
        }

        Image::Format Image::getFormat() const {
            return format_;
        }

        Image::Image(size_t width, size_t height,
            Image::Format format, boost::shared_array<unsigned char> data)
            : width_(width), height_(height), format_(format), data_(data) {
            // ignored
        }
    }
}