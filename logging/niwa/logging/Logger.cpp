/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "Logger.h"

#include "IAppender.h"
#include "Level.h"

#include "niwa/system/SpinLock.h"
#include "niwa/system/ScopedAcquire.h"

#include <set>

#include <typeinfo>

namespace {
    static const niwa::logging::Level* volatile gLevel = 
        &niwa::logging::Level::info();

    static niwa::system::SpinLock gLock;

    /**
     * Elements not owned.
     */
    static std::set<niwa::logging::IAppender*> gAppenders;
}

namespace niwa {
    namespace logging {
        using system::ScopedAcquire;

        __declspec(dllexport)
        Logger::~Logger() {
            // ignored
        }

        __declspec(dllexport)
        Logger::Logger(std::type_info const& info) : name_(info.name()) {
            // ignored
        }

        __declspec(dllexport)
        Logger::Logger(std::string const& name) : name_(name) {
            // ignored
        }

        __declspec(dllexport)
        Logger::Line Logger::debug() {
            return Line(*this, Level::debug());
        }

        __declspec(dllexport)
        Logger::Line Logger::info() {
            return Line(*this, Level::info());
        }

        __declspec(dllexport)
        Logger::Line Logger::warn() {
            return Line(*this, Level::warn());
        }

        __declspec(dllexport)
        Logger::Line Logger::error() {
            return Line(*this, Level::error());
        }

        __declspec(dllexport)
        std::string const& Logger::getName() const {
            return name_;
        }

        __declspec(dllexport)
        void Logger::setLevel(Level const& level) {
            ScopedAcquire acquire(&gLock);

            gLevel = &level;
        }

        __declspec(dllexport)
        void Logger::addAppender(IAppender* appender) {
            ScopedAcquire acquire(&gLock);

            gAppenders.insert(appender);
        }

        __declspec(dllexport)
        void Logger::removeAppender(IAppender* appender) {
            ScopedAcquire acquire(&gLock);

            gAppenders.erase(appender);
        }

        void Logger::append(
                Logger const& logger,
                Level const& level, std::string const& str) {
            ScopedAcquire acquire(&gLock);

            std::set<IAppender*>::iterator it = gAppenders.begin();

            for(; it != gAppenders.end(); ++it) {
                (*it)->append(logger, level, str);
            }
        }

        __declspec(dllexport)
        Logger::Line::~Line() {
            if(isUsed_) {
                std::string str = stream_.str();

                Logger::append(logger_, level_, str);
            }
        }

        Logger::Line::Line(Logger const& logger, Level const& level) 
            : logger_(logger), level_(level), 
              isUsed_(level.getLevel() >= gLevel->getLevel()) {
            // ignored
        }
    }
}
