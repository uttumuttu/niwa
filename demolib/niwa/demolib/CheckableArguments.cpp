/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "CheckableArguments.h"

extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

namespace niwa {
    namespace demolib {
        CheckableArguments::CheckableArguments(lua_State* L) : L_(L) {
            // ignored
        }

        int CheckableArguments::error(char const* fmt, ...) const {
            // Copy-pasted from luaL_error (version 5.1.2).
            va_list argp;
            va_start(argp, fmt);
            luaL_where(L_, 1);
            lua_pushvfstring(L_, fmt, argp);
            va_end(argp);
            lua_concat(L_, 2);
            return lua_error(L_);
        }

        int CheckableArguments::checkInteger(std::string const& name) const {
            if(get(name).isNumber()) {
                return get(name).asNumber<int>();
            } else {
                error("expected integer '%s'", name.c_str());
                return 0;
            }
        }

        std::string CheckableArguments::checkString(std::string const& name) const {
            if(get(name).isString()) {
                return get(name).asString();
            } else {
                error("expected string '%s'", name.c_str());
                return "";
            }
        }

        boost::shared_ptr<LuaRef> CheckableArguments::checkObject(std::string const& name) const {
            if(get(name).isObject()) {
                return get(name).asObject();
            } else {
                error("expected object '%s'", name.c_str());
                return boost::shared_ptr<LuaRef>();
            }
        }

        std::vector<boost::shared_ptr<LuaRef>> CheckableArguments::checkObjectList(std::string const& name) const {
            if(get(name).isObjectList()) {
                return get(name).asObjectList();
            } else {
                error("expected object list '%s'", name.c_str());
                return std::vector<boost::shared_ptr<LuaRef>>();
            }
        }
    }
}