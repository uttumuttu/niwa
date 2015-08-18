/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DEMOLIB_OBJECTPROVIDER_H
#define NIWA_DEMOLIB_OBJECTPROVIDER_H

#include <string>

#include <boost/shared_ptr.hpp>

struct lua_State;

namespace niwa {
    namespace demolib {
        class CheckableArguments;
        class IObject;

        /**
         * A provider (factory) of demo objects.
         */
        class ObjectProvider {
        public:
            /**
             * Creates a provider for an argument-free type.
             */
            ObjectProvider(
                std::string const& name,
                IObject* (*factory)());

            /**
             * Creates a provider for a type taking arguments.
             */
            ObjectProvider(
                std::string const& name,
                IObject* (*factory)(CheckableArguments const&));

            /**
             * @return Name of the provided type of object.
             */
            std::string getName() const;

            /**
             * @return The created object; never NULL.
             */
            IObject* createObject(CheckableArguments const& args) const;

            bool operator < (ObjectProvider const& rhs) const;

        private:
            class IFactory;

        private:
            std::string name_;

            boost::shared_ptr<IFactory> factory_;
        };
    }
}

#endif