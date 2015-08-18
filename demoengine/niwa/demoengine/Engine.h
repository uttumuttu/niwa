/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DEMOENGINE_ENGINE_H
#define NIWA_DEMOENGINE_ENGINE_H

#include "Span.h"

#include <vector>

struct lua_State;

namespace niwa {
    namespace demolib {
        class IObject;
    }

    namespace demoengine {
        /**
         * Runs demos.
         */
        class Engine {
        public:
            explicit Engine(lua_State* L);

            void addSpan(Span const& span);

            /**
             * @return Whether initialization succeeded.
             */
            bool initOpenGl(int argc, char** argv);

            void start();

        private:
            /**
             * @return Active spans sorted by ascending layer.
             */
            std::vector<Span> getActiveSpans(double timeSeconds);

            /**
             * Setups spans that have started but have not been setup yet.
             */
            void setupSpans(double timeSeconds);

            static void render();

            static void processNormalKeys(unsigned char key, int x, int y);

            static void processSpecialKeys(int key, int x, int y);

        private: // prevent copying
            Engine(Engine const&);
            Engine& operator = (Engine const&);

        private:
            lua_State* const L_;

            std::vector<Span> spans_;
        };
    }
}

#endif