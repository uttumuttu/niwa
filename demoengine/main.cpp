/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

/**
 * @mainpage
 *
 * Niwa (Japanese for garden) is a collection of
 * my programming efforts of personal interest.
 */

extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

#include "niwa/demoengine/Engine.h"

#include "niwa/demolib/Arguments.h"
#include "niwa/demolib/ArgumentError.h"
#include "niwa/demolib/CheckableArguments.h"
#include "niwa/demolib/ObjectProviderRegistry.h"
#include "niwa/demolib/IEffect.h"
#include "niwa/demolib/LuaRef.h"

#include "niwa/system/ProcessorInfo.h"

#include "niwa/logging/Level.h"
#include "niwa/logging/Logger.h"
#include "niwa/logging/ScopedAppender.h"
#include "niwa/logging/appenders/StreamAppender.h"

using namespace niwa::demolib;
using namespace niwa::demoengine;

#include <iostream>

#include <windows.h>

#define DEFAULT_LAYER 0

namespace {
    static niwa::logging::Logger log("main");

    static Engine* gEngine = 0;

    static ObjectProviderRegistry* gRegistry = 0;

    /**
     * Parses arguments.
     */
    static void parse_args(lua_State *const L, Arguments& args) {
        const int top = lua_gettop(L); // table position

        for(lua_pushnil(L); lua_next(L, top); lua_pop(L, 1)) {
            char const* name = luaL_checkstring(L, -2);

            try {
                args.set(name, Argument::parseThrowing(L, true));
            } catch(ArgumentError const& ae) {
                std::string message = ae.getMessage();

                luaL_error(L, message.c_str());
            }
        }
    }

    /**
     * Creates a demo object whose type-name is given
     * as the first upvalue. The demo object arguments
     * are given as an optional table.
     */
    static int create_object(lua_State *const L) {
        char const* name = luaL_checkstring(L, lua_upvalueindex(1));

        const int argc = lua_gettop(L);

        CheckableArguments args(L);

        if(argc == 0) {
            // Ignored.
        } else if(argc != 1) {
            luaL_error(L, "wrong number of arguments (expected 0 or 1, got %d)", argc);
        } else if(lua_istable(L, 1)) {
            parse_args(L, args);
        } else {
            luaL_error(L, "unexpected argument type");
        }

        IObject* object = gRegistry->createObject(name, args);

        log.debug() << "'" << name << "' created at " << object;

        IObject** userData = static_cast<IObject**>(
            lua_newuserdata(L, sizeof(IObject*)));

        luaL_getmetatable(L, IObject::METATABLE_NAME);
        lua_setmetatable(L, -2);

        *userData = object;
        return 1;
    }

    static int garbage_collect_object(lua_State *const L) {
        IObject** object = static_cast<IObject**>(
            luaL_checkudata(L, 1, IObject::METATABLE_NAME));

        log.debug() << "garbage collecting object at " << *object;

        delete *object;
        *object = 0;

        return 0;
    }

    static int new_index_object(lua_State *const L) {
        IObject** object = static_cast<IObject**>(
            luaL_checkudata(L, 1, IObject::METATABLE_NAME));

        char const* name = luaL_checkstring(L, 2);

        try {
            CheckableArguments args(L);

            args.set(name, Argument::parseThrowing(L, true));

            (*object)->setArguments(args);
        } catch(ArgumentError const& ae) {
            std::string message = ae.getMessage();

            luaL_error(L, message.c_str());
        }
        return 0;
    }

    /**
     * Adds new upvalue bindings to create_object
     * based on the given registry.
     */
    static void update_constructor_bindings(
        lua_State *const L, ObjectProviderRegistry const& registry) {
        std::set<ObjectProvider> const& providers = registry.getProviders();

        std::set<ObjectProvider>::const_iterator it = providers.begin();

        for(; it != providers.end(); ++it) {
            std::string name = it->getName();

            lua_pushstring(L, name.c_str());
            lua_pushcclosure(L, create_object, 1);
            lua_setglobal(L, name.c_str());
        }
    }

    /**
     * Creates a preallocated table efficiently.
     */
    static int createtable_c(lua_State *const L) {
        int narr = luaL_optint(L,1,0);
        int nrec = luaL_optint(L,2,0);

        lua_createtable(L, narr, nrec);
        return 1;
    }

    static int load_dll(lua_State *const L) {
        const int argc = lua_gettop(L);

        if(argc != 1) {
            luaL_error(L, "wrong number of arguments (expected 1, got %d)", argc);
        } 

        const char* file = luaL_checkstring(L, 1);

        log.info() << "loading " << file;
        
        HMODULE module = LoadLibrary(file);

        if(module) {
            log.debug() << "registering providers";

            void (*registerProviders)(ObjectProviderRegistry&) = 
                reinterpret_cast<void (*)(ObjectProviderRegistry&)>(
                    GetProcAddress(module, "registerProviders"));

            if(registerProviders) {
                ObjectProviderRegistry registry;

                registerProviders(registry);

                gRegistry->registerProviders(registry);

                update_constructor_bindings(L, registry);
            } else {
                log.error() << "registering providers failed (callback not found)";
            }
        } else {
            log.error() << "loading failed";
        }

        return 0;
    }

    static int add_span(lua_State *const L) {
        const int argc = lua_gettop(L);

        if(argc != 1) {
            luaL_error(L, "wrong number of arguments (expected 1, got %d)", argc);
        } else if(!lua_istable(L, 1)) {
            luaL_error(L, "expected table input");
        }

        const int top = lua_gettop(L);

        // read effect reference (mandatory argument)
        lua_pushnumber(L, 1);
        lua_gettable(L, top);
        luaL_checkudata(L, -1, IObject::METATABLE_NAME);
        boost::shared_ptr<LuaRef> effect(new LuaRef(L));
        lua_pushnil(L); // popped by reference creation
        lua_pop(L, 1);

        // read start time (mandatory argument)
        lua_pushnumber(L, 2);
        lua_gettable(L, top);
        double startTimeSeconds = luaL_checknumber(L, -1);
        lua_pop(L, 1);

        // read end time (mandatory argument)
        lua_pushnumber(L, 3);
        lua_gettable(L, top);
        double endTimeSeconds = luaL_checknumber(L, -1);
        lua_pop(L, 1);

        int layer = DEFAULT_LAYER;

        boost::shared_ptr<LuaRef> setupCallback(
            LuaRef::createNonValid());

        // finally, parse optional arguments
        for(lua_pushnil(L); lua_next(L,top); lua_pop(L, 1)) {
            if(lua_isnumber(L,-2)) {
                int key = lua_tointeger(L,-2);

                if(key < 1 || key > 3) {
                    luaL_error(L, "expected only named optional arguments");
                }
            } else if(lua_isstring(L,-2)) {
                std::string key = lua_tostring(L,-2);

                if(key == "layer") {
                    layer = luaL_checkinteger(L, -1);
                } else if(key == "setup") {
                    if(lua_isfunction(L,-1)) {
                        setupCallback = boost::shared_ptr<LuaRef>(new LuaRef(L));

                        lua_pushnil(L); // was popped by setup
                    } else {
                        luaL_error(L, "'setup' must be a function");
                    }
                } else {
                    luaL_error(L, "unrecognized optional argument '%s'", key);
                }
            } else {
                luaL_error(L, "unsupported optional argument type");
            }
        }

        if(gEngine) {
            gEngine->addSpan(Span(
                effect, startTimeSeconds, endTimeSeconds,
                layer, setupCallback));
        }

        return 0;
    }

    /**
     * The engine is run in a separate function from main
     * to ensure that the lifetime of the lua VM exceeds
     * that of the engine. (That is, the engine can safely
     * make lua callbacks during its whole existence.)
     */
    static int runEngine(lua_State* L, int argc, char** argv) {
        static const luaL_reg objectMeta[] = {
            {"__gc", garbage_collect_object},
            {"__newindex", new_index_object},
            {0, 0}
        };

        Engine engine(L);
        ObjectProviderRegistry registry;

        gEngine = &engine;
        gRegistry = &registry;

        if(!engine.initOpenGl(argc, argv)) {
            std::cerr << "OpenGL initialization failed." << std::endl;
            return -1;
        }

        if(argc != 2) {
            std::cerr << "Error: expected lua script as command line argument." << std::endl;
            return -1;
        }

        luaL_openlibs(L);

        luaL_newmetatable(L, IObject::METATABLE_NAME);
        luaL_openlib(L, 0, objectMeta, 0); // fill metatable

        lua_register(L, "createtable_c", createtable_c);

        lua_register(L, "load_dll", load_dll);
        lua_register(L, "add_span", add_span);

        if(luaL_loadfile(L, argv[1]) 
            || lua_pcall(L, 0, LUA_MULTRET, 0)
            || lua_gc(L, LUA_GCCOLLECT,0)) {
            std::cerr << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1); // error message
            return -1;
        }  else {
            niwa::system::ProcessorInfo info = 
                niwa::system::ProcessorInfo::create();

            log.info() << "logical processor count: "
                << info.getLogicalProcessorCount();

            engine.start();
            return 0;
        }
    }
}

int main(int argc, char **argv) {
    niwa::logging::appenders::StreamAppender appender(std::cout);

    niwa::logging::ScopedAppender scopedAppend(&appender);

    niwa::logging::Logger::setLevel(niwa::logging::Level::debug());

    lua_State* L = luaL_newstate();

    if(!L) {
        std::cerr << "Error: cannot initialize lua." << std::endl;

        return -1;
    } else {
        int result = runEngine(L, argc, argv);

        lua_close(L);
        return result;
    }
}