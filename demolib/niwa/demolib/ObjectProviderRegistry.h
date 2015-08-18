/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DEMOLIB_OBJECTPROVIDERREGISTRY_H
#define NIWA_DEMOLIB_OBJECTPROVIDERREGISTRY_H

#include <string>
#include <set>

#include "ObjectProvider.h"

namespace niwa {
    namespace demolib {
        class CheckableArguments;

        class IObject;

        /**
         * A registry for providers (factories)
         * that each create demo objects of a single type.
         */
        class ObjectProviderRegistry {
        public:
            ObjectProviderRegistry();
            ~ObjectProviderRegistry();

            void registerProvider(
                ObjectProvider const& provider);

            void unregisterProvider(
                ObjectProvider const& provider);

            /**
             * Registers providers from another registry.
             */
            void registerProviders(
                ObjectProviderRegistry const& other);

            std::set<ObjectProvider> const& getProviders() const;

            /**
             * @return The created object; never NULL.
             */
            IObject* createObject(
                std::string const& name, CheckableArguments const& args) const;

        private: // prevent copying
            ObjectProviderRegistry(ObjectProviderRegistry const&);
            ObjectProviderRegistry& operator = (ObjectProviderRegistry const&);

        private:
            std::set<ObjectProvider> providers_;
        };
    }
}

#endif