/**
 * @file
 * @param
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DEMOLIB_IGRAPHICS_H
#define NIWA_DEMOLIB_IGRAPHICS_H

#include <utility>

namespace niwa {
    namespace demolib {
        /**
         * Graphical arguments passed to an effect.
         */
        class IGraphics {
        public:
            virtual ~IGraphics();

            virtual size_t getWidth() const = 0;
            virtual size_t getHeight() const = 0;

            virtual std::pair<size_t, size_t> getDimensions() const = 0;

            /**
             * @return Width divided by height, or zero if height is equal to zero.
             */
            virtual double getAspectRatio() const = 0;
        };
    }
}

#endif