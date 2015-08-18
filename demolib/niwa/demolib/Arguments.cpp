/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "Arguments.h"

#include "ArgumentError.h"
#include "CheckableArguments.h"

#include "LuaRef.h"

extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

#include "niwa/logging/Logger.h"

using std::string;
using std::map;

/**
 * The time value to use if none is given.
 */
#define DEFAULT_TIME 0.0f

namespace niwa {
    namespace demolib {
        static logging::Logger log(typeid(Arguments));

        Arguments::Arguments() {
            // ignored
        }

        Arguments::~Arguments() {
            // ignored
        }

        Arguments::Arguments(CheckableArguments const& args)
        : map_(args.map_) {
            // ignored
        }

        static Argument callFuture(
                boost::shared_ptr<LuaRef> future, double timeSeconds,
                Argument const& errorValue) {
            if(!future->isValid()) {
                return errorValue;
            }

            future->pushValue();

            lua_State* L = future->getLua();

            lua_pushnumber(L, timeSeconds);

            if(lua_pcall(L, 1, 1, 0) != 0) {
                log.warn() << "callback failed, reason: "
                    << lua_tostring(L, -1);

                lua_pop(L, 1); // error message

                return errorValue;
            }
            
            const int top = lua_gettop(L);

            if(top == 0) {
                log.error() << "missing return value";
                return errorValue;
            }

            try {
                return Argument::parseThrowing(L, false);
            } catch(ArgumentError const& ae) {
                log.error() << ae.getMessage();
                return errorValue;
            }
        }

        void Arguments::set(string const& name, Argument const& value) {
            if(map_.find(name) != map_.end()) {
                map_.erase(name);
            }
            map_.insert(MapValue(name, value));
        }

        size_t Arguments::size() const {
            return map_.size();
        }

        std::string Arguments::nameAt(size_t index) const {
            ConstIterator it = map_.begin();

            for(size_t i=0; i<index; ++i) {
                ++it;
            }
            return it->first;
        }

        void Arguments::set(Arguments const& args) {
            ConstIterator it = args.map_.begin();

            for(; it != args.map_.end(); ++it) {
                std::string const& name = it->first;

                if(map_.find(name) != map_.end()) {
                    map_.erase(name);
                }

                map_.insert(MapValue(name, it->second));
            }
        }

        Argument Arguments::get(std::string const& name) const {
            return getAtTime(name, DEFAULT_TIME);
        }

        Argument Arguments::getAtTime(
            std::string const& name, double timeSeconds) const {
            ConstIterator it = map_.find(name);

            if(it != map_.end()) {
                Argument const& arg = it->second;

                if(arg.type() != Argument::FUTURE) {
                    return arg;
                } else {
                    boost::shared_ptr<LuaRef> future(
                        arg.future_);

                    return callFuture(future, timeSeconds, Argument());
                }
            } else {
                return Argument();
            }
        }
    }
}
