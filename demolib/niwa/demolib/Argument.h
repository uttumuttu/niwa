/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DEMOLIB_ARGUMENT_H
#define NIWA_DEMOLIB_ARGUMENT_H

#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>

struct lua_State;

namespace niwa {
    namespace demolib {
        class LuaRef;

        /**
         * A copy of an argument (at a given time)
         * passed to the demo library.
         *
         * Vectors, matrices and lists are guaranteed to have
         * positive dimensions. All matrix data are stored
         * in row-major format.
         */
        class Argument {
        public:
            /**
            * An array wrapper for passing statically-sized vector-typed argument data.
            *
            * Supports implicit casting to and from basic arrays for convenience.
            */
            template <typename T, size_t N>
            class Array {
            public:
                /**
                * Creates an array of zeroes.
                */
                Array();

                /**
                * Creates an array from given values.
                */
                Array(T const* values);

                operator T * ();

                operator T const* () const;

            private:
                T values_[N];
            };

        public:
            enum Type {
                UNDEFINED,
                NUMBER,
                STRING,
                VECTOR,
                MATRIX,
                OBJECT,
                OBJECT_LIST,
                /**
                 * Internal value for implementing time-dependent variables.
                 * Clients that read arguments from an Arguments object will
                 * never see Argument objects of this type.
                 */
                FUTURE
            };

            /**
             * @return Type of the argument.
             */
            Type type() const;

        public: // testing
            bool isNumber() const;

            bool isString() const;

            bool isVector() const;

            bool isVector(size_t dimension) const;

            bool isMatrix() const;

            bool isMatrix(size_t nRows, size_t nColumns) const;

            bool isObject() const;

            bool isObjectList() const;

        public: // getters
            template <typename T> T asNumber() const;

            template <typename T> T asNumber(T defaultValue) const;

            std::string asString() const;

            template <typename T> std::vector<T> asVector() const;

            template <typename T, size_t N> Array<T,N> asVector() const;

            template <typename T, size_t N> Array<T,N> asVector(
                Array<T,N> const& defaultValue) const;

            template <typename T> std::vector<T> asMatrix() const;

            boost::shared_ptr<LuaRef> asObject() const;

            std::vector<boost::shared_ptr<LuaRef>> asObjectList() const;

            size_t vectorDimension() const;

            std::pair<size_t, size_t> matrixDimensions() const;

        public: // parsing
            /**
             * Parses a single, defined argument from the top of the lua stack.
             *
             * @todo Consider using Lua auxilliary library's error handling.
             *
             * @throw ArgumentError
             */
            static Argument parseThrowing(lua_State* L, bool allowFutures);

        public:
            explicit Argument(double number);

            explicit Argument(std::string const& text);

            explicit Argument(std::vector<double> vector);

            Argument(
                std::pair<size_t, size_t> dimensions,
                std::vector<double> matrix);

            /**
             * @param type Either OBJECT for FUTURE.
             */
            Argument(Type type,
                boost::shared_ptr<LuaRef> object);

            explicit Argument(
                std::vector<boost::shared_ptr<LuaRef>> objects);

        private:
            Argument(); /** Creates a nonexistent argument. */

        private:
            Type type_;

            std::vector<double> numbers_;

            std::string text_;

            std::pair<size_t, size_t> dimensions_;

            std::vector<boost::shared_ptr<LuaRef>> objects_;

            boost::shared_ptr<LuaRef> future_;

            friend class Arguments;
        };
    }
}

#include "Argument.inl"

#endif