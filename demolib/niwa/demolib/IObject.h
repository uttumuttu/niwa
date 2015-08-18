/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_DEMOLIB_IOBJECT_H
#define NIWA_DEMOLIB_IOBJECT_H

namespace niwa {
    namespace demolib {
        class CheckableArguments;
        class IGraphics;

        /**
         * A managed demo object.
         */
        class IObject {
        public:
            static char const* METATABLE_NAME;

        public:
            IObject();

            virtual ~IObject();

            /**
             * Sets argument values. May be called multiple
             * times after the object's construction.
             *
             * Costly arguments (e.g., filenames for 3D models)
             * don't have to be acted upon in setArguments.
             * Arguments for such "one-shot" operations should
             * be provided to the object at construction-time.
             */
            virtual void setArguments(
                CheckableArguments const& args) = 0;

        private: // prevent copying and slicing
            IObject(IObject const&);
            IObject& operator = (IObject const&);
        };
    }
}

#endif