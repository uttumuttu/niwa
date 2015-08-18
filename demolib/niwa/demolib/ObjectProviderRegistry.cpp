/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "ObjectProviderRegistry.h"

#include "ObjectProvider.h"

#include "CheckableArguments.h"

#include "niwa/logging/Logger.h"

namespace niwa {
    namespace demolib {
        static niwa::logging::Logger log(typeid(ObjectProviderRegistry));

        ObjectProviderRegistry::ObjectProviderRegistry() {
            // ignored
        }

        ObjectProviderRegistry::~ObjectProviderRegistry() {
            // ignored
        }

        std::set<ObjectProvider> const& ObjectProviderRegistry::getProviders() const {
            return providers_;
        }

        void ObjectProviderRegistry::registerProvider(ObjectProvider const& provider) {
            log.debug() << "registering provider for " << provider.getName();
            providers_.insert(provider);
        }

        void ObjectProviderRegistry::unregisterProvider(ObjectProvider const& provider) {
            log.debug() << "unregistering provider for " << provider.getName();
            providers_.erase(provider);
        }

        void ObjectProviderRegistry::registerProviders(ObjectProviderRegistry const& other) {
            std::set<ObjectProvider>::const_iterator provider;

            for(provider=other.providers_.begin();
                provider!=other.providers_.end(); ++provider) {
                // avoid double logging
                providers_.insert(*provider);
            }
        }

        IObject* ObjectProviderRegistry::createObject(
            std::string const& name, CheckableArguments const& args) const {
            std::set<ObjectProvider>::const_iterator provider;

            for(provider=providers_.begin(); 
                provider!=providers_.end(); ++provider) {
                if(provider->getName() == name) {
                    return provider->createObject(args);
                }
            }

            args.error("cannot create '%s' (missing provider)", name);
            return 0;
        }
    }
}
