/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

/**
 * @namespace niwa::inject Dependency injection framework inspired by Google Guice.
 */

#ifndef NIWA_INJECT_INJECT_H
#define NIWA_INJECT_INJECT_H

#include "IInjectProvider.h"

#define INJECT0(T) \
    class InjectProvider : public IInjectProvider<T> { public: \
        T* newInstance(Injector const& /*injector*/) const { \
            return new T(); } };

#define INJECT1(T, A1) \
    class InjectProvider : public IInjectProvider<T> { public: \
        T* newInstance(Injector const& injector) const { \
            A1* a1 = injector.newInstance<A1>(); \
            return new T(a1); } };

#define INJECT2(T, A1, A2) \
    class InjectProvider : public IInjectProvider<T> { public: \
        T* newInstance(Injector const& injector) const { \
        A1* a1 = injector.newInstance<A1>(); \
        A2* a2 = injector.newInstance<A2>(); \
        return new T(a1, a2); } };

#define INJECT3(T, A1, A2, A3) \
    class InjectProvider : public IInjectProvider<T> { public: \
        T* newInstance(Injector const& injector) const { \
        A1* a1 = injector.newInstance<A1>(); \
        A2* a2 = injector.newInstance<A2>(); \
        A3* a3 = injector.newInstance<A3>(); \
        return new T(a1, a2, a3); } };

#define INJECT4(T, A1, A2, A3, A4) \
    class InjectProvider : public IInjectProvider<T> { public: \
        T* newInstance(Injector const& injector) const { \
        A1* a1 = injector.newInstance<A1>(); \
        A2* a2 = injector.newInstance<A2>(); \
        A3* a3 = injector.newInstance<A3>(); \
        A4* a4 = injector.newInstance<A4>(); \
        return new T(a1, a2, a3, a4); } };

#endif