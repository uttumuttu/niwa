/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DEMOLIB_LUAREF_H
#define NIWA_DEMOLIB_LUAREF_H

struct lua_State;

namespace niwa {
    namespace demolib {
        class IObject;

        /**
         * A managed, unique and non-shared reference to a lua object.
         * Uses LUA_REGISTRYINDEX to index the object.
         */
        class LuaRef {
        public:
            /**
             * Creates a non-valid reference.
             */
            static LuaRef* createNonValid();

            /**
             * The constructor assumes that the object
             * to be referenced is at the top of the
             * stack. Upon completion, this object
             * is popped from the stack.
             *
             * @param L Lifespan of L must exceed that of the LuaRef object.
             */
            explicit LuaRef(lua_State* L);

            ~LuaRef();

            /**
             * The returned Lua state should be used
             * for pushing arguments to a referenced function.
             *
             * Behavior undefined for non-valid references.
             */
            lua_State* getLua() const;

            /**
             * @return Whether the reference is valid.
             */
            bool isValid() const;

            /**
             * Pushes the reference onto the stack.
             * Behavior undefined for non-valid references.
             */
            void pushValue() const;

            /**
             * @return Assuming the reference is to an IObject,
             *         returns a pointer to it. If the reference
             *         is not valid, returns NULL.
             */
            IObject* getObject() const;

        private:
            LuaRef(int ref, lua_State* L);

        private: // prevent copying
            LuaRef(LuaRef const&);
            LuaRef& operator = (LuaRef const&);

        private:
            int const ref_;

            lua_State* const L_;
        };
    }
}

#endif