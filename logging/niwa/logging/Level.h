/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_LOGGING_LEVEL_H
#define NIWA_LOGGING_LEVEL_H

#include <string>

namespace niwa {
    namespace logging {
        class Level {
        public:
            __declspec(dllexport) static Level const& all();
            __declspec(dllexport) static Level const& debug();
            __declspec(dllexport) static Level const& info();
            __declspec(dllexport) static Level const& warn();
            __declspec(dllexport) static Level const& error();
            __declspec(dllexport) static Level const& off();

            int getLevel() const;

            std::string const& getName() const;

        private:
            static const Level all_;
            static const Level debug_;
            static const Level info_;
            static const Level warn_;
            static const Level error_;
            static const Level off_;

        private:
            Level(int level, std::string const& name);

        private: // prevent copying
            Level(Level const&);
            Level& operator = (Level const&);

        private:
            int level_;
            std::string name_;
        };
    }
}

#endif