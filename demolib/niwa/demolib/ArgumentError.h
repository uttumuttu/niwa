/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DEMOLIB_ARGUMENTERROR_H
#define NIWA_DEMOLIB_ARGUMENTERROR_H

#include <string>

namespace niwa {
    namespace demolib {
        /**
         * An exception related to demo arguments.
         * A rare case of exceptions in the project's
         * public interface; use with care.
         *
         * @todo Consider using return values for argument error handling.
         */
        class ArgumentError {
        public:
            explicit ArgumentError(std::string const& message);

            std::string const& getMessage() const;

        private: // prevent assignment (enforce catch by reference)
            ArgumentError& operator = (ArgumentError const&);

        private: // prevent naive heap allocation (enforce throw by value)
            void* operator new (size_t size);
            void operator delete (void*);

        private:
            std::string message_;
        };
   }
}

#endif