/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "Level.h"

namespace niwa {
    namespace logging {
        const Level Level::all_(1, "ALL");
        const Level Level::debug_(2, "DEBUG");
        const Level Level::info_(3, "INFO");
        const Level Level::warn_(4, "WARN");
        const Level Level::error_(5, "ERROR");
        const Level Level::off_(6, "OFF");

        __declspec(dllexport) Level const& Level::all() {
            return all_;
        }
        __declspec(dllexport) Level const& Level::debug() {
            return debug_;
        }
        __declspec(dllexport) Level const& Level::info() {
            return info_;
        }
        __declspec(dllexport) Level const& Level::warn() {
            return warn_;
        }
        __declspec(dllexport) Level const& Level::error() {
            return error_;
        }
        __declspec(dllexport) Level const& Level::off() {
            return off_;
        }

        int Level::getLevel() const {
            return level_;
        }

        std::string const& Level::getName() const {
            return name_;
        }

        Level::Level(int level, std::string const& name)
            : level_(level), name_(name) {
            // ignored
        }
    }
}