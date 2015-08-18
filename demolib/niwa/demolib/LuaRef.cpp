/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "LuaRef.h"

#include "IObject.h"

extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

#include <cassert>

#include "niwa/logging/Logger.h"

namespace niwa {
    namespace demolib {
        static logging::Logger log(typeid(LuaRef));

        LuaRef* LuaRef::createNonValid() {
            return new LuaRef(LUA_NOREF, 0);
        }

        LuaRef::LuaRef(lua_State* L) 
            : ref_(luaL_ref(L, LUA_REGISTRYINDEX)), L_(L) {
            //log.debug() << "created reference to " << ref_;
        }

        LuaRef::LuaRef(int ref, lua_State* L)
            : ref_(ref), L_(L) {
            assert(ref == LUA_NOREF);
        }

        LuaRef::~LuaRef() {
            if(ref_ != LUA_NOREF) {
                //log.debug() << "deleting reference to " << ref_;
                luaL_unref(L_, LUA_REGISTRYINDEX, ref_);
            }
        }

        bool LuaRef::isValid() const {
            return ref_ != LUA_NOREF;
        }

        lua_State* LuaRef::getLua() const {
            return L_;
        }

        void LuaRef::pushValue() const {
            if(isValid()) {
                lua_rawgeti(L_, LUA_REGISTRYINDEX, ref_);
            } else {
                assert(false);
            }
        }

        IObject* LuaRef::getObject() const {
            if(!isValid()) {
                return 0;
            }

            lua_rawgeti(L_, LUA_REGISTRYINDEX, ref_);

            IObject** userData = static_cast<IObject**>(
                luaL_checkudata(L_,-1,IObject::METATABLE_NAME));

            lua_pop(L_,1);

            return *userData;
        }
    }
}
