/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "Argument.h"

#include "ArgumentError.h"

#include "LuaRef.h"

extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

#include "niwa/logging/Logger.h"

namespace niwa {
    namespace demolib {
        static logging::Logger Log(typeid(Argument));

        Argument::Type Argument::type() const {
            return type_;
        }

        bool Argument::isNumber() const {
            return type_ == NUMBER;
        }

        bool Argument::isString() const {
            return type_ == STRING;
        }

        bool Argument::isVector() const {
            return type_ == VECTOR;
        }

        bool Argument::isVector(size_t dimension) const {
            return type_ == VECTOR && numbers_.size() == dimension;
        }

        bool Argument::isMatrix() const {
            return type_ == MATRIX;
        }

        bool Argument::isMatrix(size_t nRows, size_t nColumns) const {
            return type_ == MATRIX 
                && dimensions_ == std::pair<size_t, size_t>(nRows, nColumns);
        }

        bool Argument::isObject() const {
            return type_ == OBJECT;
        }

        bool Argument::isObjectList() const {
            return type_ == OBJECT_LIST;
        }

        std::string Argument::asString() const {
            return type_ == STRING ? text_ : "";
        }

        boost::shared_ptr<LuaRef> Argument::asObject() const {
            if(type_ == OBJECT) {
                return objects_[0];
            } else {
                boost::shared_ptr<LuaRef> dummy(LuaRef::createNonValid());

                return dummy;
            }
        }

        std::vector<boost::shared_ptr<LuaRef>> Argument::asObjectList() const {
            if(type_ == OBJECT_LIST) {
                return objects_;
            } else {
                return std::vector<boost::shared_ptr<LuaRef>>();
            }
        }

        Argument::Argument() : type_(UNDEFINED) {
            // ignored
        }

        Argument::Argument(double number) : type_(NUMBER) {
            numbers_.push_back(number);
        }
        
        Argument::Argument(std::string const& text) : type_(STRING), text_(text) {
            // ignored
        }

        Argument::Argument(std::vector<double> vector) : type_(VECTOR), numbers_(vector) {
            // ignored
        }

        Argument::Argument(
            std::pair<size_t, size_t> dimensions,
            std::vector<double> matrix) 
            : type_(MATRIX), dimensions_(dimensions), numbers_(matrix) {
            // ignored
        }

        Argument::Argument(Type type, boost::shared_ptr<LuaRef> object)
            : type_(type) {
            switch(type) {
            case OBJECT:
                objects_.push_back(object);
                break;
            case FUTURE:
                future_ = object;
                break;
            default:
                Log.error() << "expected OBJECT of FUTURE";
            }
        }

        Argument::Argument(std::vector<boost::shared_ptr<LuaRef>> objects) 
            : type_(OBJECT_LIST), objects_(objects) {
            // ignored
        }

        size_t Argument::vectorDimension() const {
            return type_ == VECTOR ? numbers_.size() : 0;
        }

        std::pair<size_t, size_t> Argument::matrixDimensions() const {
            return type_ == MATRIX ? dimensions_ : std::pair<size_t, size_t>(0,0);
        }

        Argument Argument::parseThrowing(lua_State *const L, bool const allowFutures) {
            if(lua_isnumber(L, -1)) {
                return Argument(lua_tonumber(L, -1));
            } else if(lua_isstring(L, -1)) {
                return Argument(lua_tostring(L, -1));
            } else if(lua_istable(L, -1)) {
                std::vector<double> numbers;
                std::vector<boost::shared_ptr<LuaRef>> objects;

                // We don't know the type of the table elements yet.
                Argument::Type type = Argument::UNDEFINED;

                const int vecTop = lua_gettop(L);

                int nColumns = -1; // only used for matrices

                for(lua_pushnil(L); lua_next(L, vecTop); lua_pop(L, 1)) {
                    if(!lua_isnumber(L, -2)) {
                        throw ArgumentError(
                            "only numeric keys supported for vectors and matrices");
                    } else if(lua_isnumber(L, -1)) {
                        if(type != Argument::UNDEFINED
                            && type != Argument::VECTOR) {
                            luaL_error(L, "mixed-type tables not allowed");
                        }
                        type = Argument::VECTOR;
                        numbers.push_back(lua_tonumber(L, -1));
                    } else if(lua_istable(L, -1)) {
                        if(type != Argument::UNDEFINED
                            && type != Argument::MATRIX) {
                                luaL_error(L, "mixed-type tables not allowed");
                        }
                        type = Argument::MATRIX;

                        std::vector<double> row;

                        const int matTop = lua_gettop(L);

                        for(lua_pushnil(L); lua_next(L, matTop); lua_pop(L, 1)) {
                            if(!lua_isnumber(L,-2) || !lua_isnumber(L,-1)) {
                                throw ArgumentError(
                                    "only numeric matrices supported");
                            } else {
                                row.push_back(lua_tonumber(L, -1));
                            }
                        }

                        if(nColumns == -1) { // first row
                            nColumns = row.size();

                            if(nColumns == 0) {
                                throw ArgumentError("matrix dimensions must be positive");
                            }
                        } else if(nColumns != row.size()) {
                            throw ArgumentError("inconsistent matrix dimensions");
                        }

                        for(size_t i=0; i<row.size(); ++i) {
                            numbers.push_back(row[i]);
                        }
                    } else if(lua_isuserdata(L, -1)) {
                        if(type != Argument::UNDEFINED
                            && type != Argument::OBJECT_LIST) {
                                luaL_error(L, "mixed-type tables not allowed");
                        }
                        type = Argument::OBJECT_LIST;

                        boost::shared_ptr<LuaRef> object(new LuaRef(L));

                        lua_pushnil(L); // was popped by reference creation

                        objects.push_back(object);
                    } else {
                        throw ArgumentError("unsupported table element type");
                    }
                }

                switch(type) {
                case UNDEFINED:
                    throw ArgumentError("empty tables not supported");
                    break;
                case VECTOR:
                    return Argument(numbers);
                case MATRIX:
                    {
                    int nRows = numbers.size() / nColumns;

                    return Argument(
                        std::pair<size_t, size_t>(nRows, nColumns),
                        numbers);
                    }
                case OBJECT_LIST:
                    return Argument(objects);
                default:
                    assert(false);
                    return Argument();
                }
            } else if(lua_isuserdata(L, -1)) {
                boost::shared_ptr<LuaRef> object(new LuaRef(L));

                lua_pushnil(L); // was popped by reference creation

                return Argument(Argument::OBJECT, object);
            } else if(lua_isfunction(L, -1)) {
                if(!allowFutures) {
                    throw ArgumentError("futures not allowed at this stage");
                }

                boost::shared_ptr<LuaRef> future(new LuaRef(L));

                lua_pushnil(L); // was popped by reference creation

                return Argument(Argument::FUTURE, future);
            } else {
                throw ArgumentError("unsupported argument type");
            }
        }
    }
}
