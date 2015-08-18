/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DEMOLIB_ARGUMENTS_H
#define NIWA_DEMOLIB_ARGUMENTS_H

#include "Argument.h"

#include <map>

namespace niwa {
    namespace demolib {
        class CheckableArguments;

        class LuaRef;
        
        /**
         * Collection of arguments passed to a demo object.
         */
        class Arguments {
        public:
            /**
             * Creates an empty set of arguments.
             */
            Arguments();

            /**
             * Creates a set of non-checkable arguments
             * from a checkable one.
             */
            explicit Arguments(CheckableArguments const&);

            /**
             * The class supports subclassing.
             */
            virtual ~Arguments();

        public: // manipulation
            void set(std::string const& name, Argument const& value);

            /**
             * Sets values from another collection of arguments.
             */
            void set(Arguments const& args);

        public: // getters
            Argument get(std::string const& name) const;

            Argument getAtTime(std::string const& name, double timeSeconds) const;

        public: // enumeration
            size_t size() const;

            std::string nameAt(size_t index) const;

        private: // prevent copying
            Arguments(Arguments const&);
            Arguments& operator = (Arguments const&);

        private:
            typedef std::map<std::string, Argument> Map;

            typedef Map::const_iterator ConstIterator;

            typedef Map::value_type MapValue;

            Map map_;
        };
    }
}

#endif