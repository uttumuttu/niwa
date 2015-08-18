/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "ObjectProvider.h"

#include "CheckableArguments.h"

namespace niwa {
    namespace demolib {
        class ObjectProvider::IFactory {
        public:
            virtual ~IFactory();

            virtual IObject* build(CheckableArguments const&) const = 0;
        };

        ObjectProvider::IFactory::~IFactory() {
            // ignored
        }

        ObjectProvider::ObjectProvider(
                std::string const& name,
                IObject* (*factory)())
            : name_(name) {
            class Factory : public IFactory {
            public:
                explicit Factory(IObject* (*factory)()) : factory_(factory) {
                    // ignored
                }

                IObject* build(CheckableArguments const&) const {
                    return factory_();
                }

            private:
                IObject* (*factory_)();
            };

            factory_ = boost::shared_ptr<IFactory>(new Factory(factory));
        }

        ObjectProvider::ObjectProvider(
            std::string const& name,
            IObject* (*factory)(CheckableArguments const&))
            : name_(name) {
            class Factory : public IFactory {
            public:
                Factory(IObject* (*factory)(CheckableArguments const&)) : factory_(factory) {
                    // ignored
                }

                IObject* build(CheckableArguments const& args) const {
                    return factory_(args);
                }

            private:
                IObject* (*factory_)(CheckableArguments const&);
            };

            factory_ = boost::shared_ptr<IFactory>(new Factory(factory));
        }

        std::string ObjectProvider::getName() const {
            return name_;
        }

        IObject* ObjectProvider::createObject(CheckableArguments const& args) const {
            IObject* result = factory_->build(args);

            if(result) {
                return result;
            } else {
                args.error("error creating '%s' (nil object)", name_);
                return 0;
            }
        }

        bool ObjectProvider::operator < (ObjectProvider const& rhs) const {
            if(name_ < rhs.name_) {
                return true;
            } else if(name_ > rhs.name_) {
                return false;
            } else {
                return factory_ < rhs.factory_;
            }
        }
    }
}
