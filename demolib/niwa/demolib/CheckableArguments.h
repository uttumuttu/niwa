/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DEMOLIB_CHECKABLEARGUMENTS_H
#define NIWA_DEMOLIB_CHECKABLEARGUMENTS_H

#include "Arguments.h"

struct lua_State;

namespace niwa {
    namespace demolib {
        /**
         * Demo arguments with error-handling capabilities.
         * Error handling is implemented via Lua exceptions.
         */
        class CheckableArguments : public Arguments {
        public:
            explicit CheckableArguments(lua_State* L);

        public: // new functions
            /**
             * Doesn't return anything meaningful;
             * return value merely compliant with Lua error handling.
             */
            int error(char const* fmt, ...) const;

            int checkInteger(std::string const& name) const;

            std::string checkString(std::string const& name) const;

            boost::shared_ptr<LuaRef> checkObject(std::string const& name) const;

            std::vector<boost::shared_ptr<LuaRef>> checkObjectList(std::string const& name) const;

        private: // prevent copying
            CheckableArguments(CheckableArguments const&);
            CheckableArguments& operator = (CheckableArguments const&);

        private:
            lua_State* L_;
        };
    }
}

#endif