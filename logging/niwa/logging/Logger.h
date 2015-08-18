/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_LOGGING_LOGGER_H
#define NIWA_LOGGING_LOGGER_H

#include <sstream>

namespace std {
    class type_info;
}

namespace niwa {
    namespace logging {
        class IAppender;
        class Level;

        /**
         * @brief A unit of logging. 
         *
         * <h2>How to Create Loggers</h2>
         *
         * <p>By convention, each class that requires
         * logging should declare its own static Logger
         * instance in its corresponding source (.CPP) file.</p>
         * 
         * <p>Here is a usage example for
         * an imaginary class <code>niwa::fruit::Pomegranate</code>:</p>
         *
         * <p>
         *   <code>
         *     // Pomegranate.cpp<br />
         *     \#include "Pomegranate.h"<br />
         *     <br />
         *     \#include "niwa/logging/Logger.h"<br />
         *     <br />
         *     using niwa::logging::Logger;<br />
         *     <br />
         *     namespace niwa {<br />
         *     namespace fruit {<br />
         *     &nbsp;&nbsp;&nbsp;&nbsp;<i>// PREFERRED CREATION IDIOM: notice the use of type information with keyword</i> <b>typeid</b>.<br />
         *     &nbsp;&nbsp;&nbsp;&nbsp;static Logger log1(<b>typeid</b>(Pomegranate));<br />
         *     <br />
         *     &nbsp;&nbsp;&nbsp;&nbsp;<i>// NON-PREFERRED CREATION IDIOM: here is a more brittle creation without keyword</i> <b>typeid</b>.<br />
         *     &nbsp;&nbsp;&nbsp;&nbsp;static Logger log2("class niwa::fruit::Pomegranate");<br />
         *     }<br />
         *     }
         *   </code>
         * </p>
         *
         * <h2>How to Use Loggers</h2>
         *
         * Similar to Apache log4j, Loggers can be sent four levels of log messages:
         * debug, info, warn and error. Each log message consists of invoking the member
         * function with the log level's name, and sending the returned stream object
         * a line of text (not terminated by special characters such as <code>std::endl</code>).
         * Examples:
         *
         * <p><code>
         * log.info() << "this is information";<br />
         * log.debug() << "my lucky number is " << 5;<br />
         * log.warn() << "this guy is crazy: " << ourCompany.getBoss();
         * </code>/<p>
         *
         * <h2>Log Levels and Performance</h2>
         *
         * <p><i>Global log level</i> can be set via method Logger::setLevel.
         * Log messages with a log level under the global log level
         * will be ignored.</p>
         *
         * <p>In practice, this ignoring is vastly more efficient than
         * in log4j, since the stream objects can simply ignore their
         * arguments.</p>
         */
        class Logger {
        private:
            class Line {
            public:
                __declspec(dllexport)
                ~Line();

                template <typename T>
                Line& operator << (T const& obj);

            private:
                Line(Logger const& logger, Level const& level);

            private: // prevent copying
                Line(Line const&);
                Line& operator = (Line const);

            private:
                Logger const& logger_;

                Level const& level_;

                bool isUsed_;

                std::stringstream stream_;

                friend class Logger;
            };

        public:
            __declspec(dllexport)
            ~Logger();

            /**
             * @brief Creates a logger associated with a type.
             */
            __declspec(dllexport)
            explicit Logger(std::type_info const& info);

            /**
             * Creates a named logger.
             */
            __declspec(dllexport)
            explicit Logger(std::string const& name);

            __declspec(dllexport)
            static void setLevel(Level const& level);

            /**
             * @param appender Does not transfer ownership.
             */
            __declspec(dllexport)
            static void addAppender(IAppender* appender);

            /**
             * @param appender Does not transfer ownership.
             */
            __declspec(dllexport)
            static void removeAppender(IAppender* appender);

            __declspec(dllexport)
            Line debug();

            __declspec(dllexport)
            Line info();

            __declspec(dllexport)
            Line warn();

            __declspec(dllexport)
            Line error();

            __declspec(dllexport)
            std::string const& getName() const;

        private:
            /**
             * @param level Assumed to be in global logging level.
             *
             * @param line Never NULL.
             */
            static void append(
                Logger const& logger,
                Level const& level, std::string const& line);

        private:
            std::string name_;
        };
    }
}

#include "Logger.inl"

#endif