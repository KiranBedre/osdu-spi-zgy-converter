/*
Copyright 2022-2025 Schlumberger

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#ifndef TESTING

#include "../LibraryLoader/DynamicLibraryLoader.h"
#include "../Logger/LogWriterBase.h"
#include "../Logger/LogWriterDispatcher.h"
#include "../Logger/LoggerSettings.h"
#include "../Logger/MessageGSM.h"
#include "../Logger/MessagePool.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <list>
#include <memory>
#include <type_traits>

namespace segysdk::logger
{

enum class SEGYSDK_DLL_PUBLIC LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3
};

inline const char* toString(LogLevel level);

class SEGYSDK_DLL_PUBLIC Logger
{
public:
    using DynamicLibraryLoader = segysdk::library_loader::DynamicLibraryLoader;
    using loadedPlugins = std::list<std::shared_ptr<DynamicLibraryLoader>>;

    // All loaded plugins should provide the following functions:
    using createLogWriter = LogWriterBase* (*)(const LoggerSettings*);
    using deleteLogWriter = void (*)(LogWriterBase*);

    // The names of the functions should be the following:
    inline static const char* CreateLogWriterFunctionName_ = "createLogWriter";
    inline static const char* DeleteLogWriterFunctionName_ = "deleteLogWriter";

    Logger(const Logger&) = delete;
    Logger operator=(const Logger&) = delete;

    ~Logger();

    /**
     * @brief Logs a message.
     *
     * @param level Log level of the message. Message is not logged if the level is lower than the one set in the Logger
     * @param func  Name of function where message happens.
     * @param file  Name of the file where message happens.
     * @param line  Line in the file where message happens.
     * @param message  Message to be logged.
     *
     **/
    template<typename T>
    void log(LogLevel level, const char* func, const char* file, int line, T message)
    {
        std::stringstream outStream;
        outStream << message;
        log(level, func, file, line, outStream);
    }

    /**
     * @brief Logs a message.
     *
     * @param level Log level of the message. Message is not logged if the level is lower than the one set in the Logger
     * @param func  Name of function where message happens.
     * @param file  Name of the file where message happens.
     * @param line  Line in the file where message happens.
     * @param sstream  stream containing a messages to be logged.
     *
     **/
    void log(LogLevel level, const char* func, const char* file, int line, const std::stringstream& sstream);

    /**
     * @brief Logs command line arguments
     *
     * @param level Log level of the message. Message is not logged if the level is lower than the one set in the Logger
     * @param func  Name of function where message happens.
     * @param file  Name of the file where message happens.
     * @param line  Line in the file where message happens.
     * @param argc  Number of argv arguments.
     * @param argv  Command line arguments.
     *
     **/
    void log(LogLevel level, const char* func, const char* file, int line, int argc, const char** argv);

    /**
     * @brief Logs GSM Message.
     *
     * @param level Log level of the message. Message is not logged if the level is lower than the one set in the Logger
     * @param message  message to be logged.
     *
     **/
    template<typename T>
    void logGSM(LogLevel level, const T& message)
    {
        static_assert(std::is_base_of_v<MessageGSM, T>, "T is not derived from MessageGSM");
        if (isLowLogLevel(level)) {
            return;
        }

        std::unique_ptr<Message> msgToStore = std::make_unique<T>(message);
        logPool_->add(std::move(msgToStore));
    }

    void logGSM(LogLevel level, const char* message);
    void logGSM(LogLevel level, std::string message);
    void logGSM(LogLevel level, const std::stringstream& message);

    /**
     * @brief Gets an instance of the Logger.
     *
     * @returns Static object of the Logger to log messages.
     *
     **/
    static Logger& getInstance();

    /**
     * @brief Sets a LogLevel of the Logger.
     *
     * @param level Is the minimum logging level of the Logger at which messages should be written.
     *
     **/
    static void setLogLevel(LogLevel level);

    /**
     * @brief Adds LogWriter to the Logger that is used to write messages.
     *
     * @param writer pointer to the object that writes the incoming messages from the Logger.
     *
     **/
    void addLogWriterToDispatcher(std::shared_ptr<LogWriterBase> writer);

    /**
     * @brief Adds LogWriter to the Logger that is used to write messages.
     *
     * @param loader pointer to the object which loads shared object into the executable and provides an interface to the internal LogWriterBase object.
     * @param settings are the logger settings with all needed params for the LogWriterBase object that should be loaded by loader.
     *
     **/
    void addLogWriterPlugin(std::shared_ptr<DynamicLibraryLoader> loader, const LoggerSettings& settings);

    /**
     * @brief Starts the Dispatcher responsible for spawning log writer threads.
     *        Should be called once the program is started and LogWriterBase objects are added.
     *
     **/
    void runLogWriterDispatcher();

    /**
     * @brief Stop the Dispatcher and log writers. No messages are written after.
     *
     **/
    void stopLogWriterDispatcher();

private:
    bool isLowLogLevel(LogLevel level);
    Logger();

    loadedPlugins logWriterPlugins_;
    static LogLevel level_;
    std::shared_ptr<MessagePool> logPool_;
    LogWriterDispatcher dispatcher_;
};

/**
 * @brief Function that simplifies initialization of the Logger according to the provided settings.
 *
 * @param settings are the logger settings to be used by Logger and internally loaded plugins (if any).
 *
 * @returns Logger to be used for logging messages.
 *
 **/
SEGYSDK_DLL_PUBLIC Logger& initializeLogger(const LoggerSettings& settings);

} // namespace segysdk::logger

#ifdef SEGYLOG_DISABLE

#define SEGYLOG(Level, message)
#define SEGYLOG_ARGS(argc, argv)
#define SEGYLOG_DEBUG(message)
#define SEGYLOG_INFO(message)
#define SEGYLOG_WARNING(message)
#define SEGYLOG_ERROR(message)
#define SEGYLOG_GSM_DEBUG(message)
#define SEGYLOG_GSM_INFO(message)

#else

#define SEGYLOG(Level, message) \
    segysdk::logger::Logger::getInstance().log((Level), __FUNCTION__, __FILE__, __LINE__, (message))
#define SEGYLOG_ARGS(argc, argv) \
    segysdk::logger::Logger::getInstance().log((segysdk::logger::LogLevel::Debug), __FUNCTION__, __FILE__, __LINE__, (argc), (argv))
#define SEGYLOG_DEBUG(message) \
    segysdk::logger::Logger::getInstance().log((segysdk::logger::LogLevel::Debug), __FUNCTION__, __FILE__, __LINE__, (message))
#define SEGYLOG_INFO(message) \
    segysdk::logger::Logger::getInstance().log((segysdk::logger::LogLevel::Info), __FUNCTION__, __FILE__, __LINE__, (message))
#define SEGYLOG_WARNING(message) \
    segysdk::logger::Logger::getInstance().log((segysdk::logger::LogLevel::Warning), __FUNCTION__, __FILE__, __LINE__, (message))
#define SEGYLOG_ERROR(message) \
    segysdk::logger::Logger::getInstance().log((segysdk::logger::LogLevel::Error), __FUNCTION__, __FILE__, __LINE__, (message))

// GSM Related
#define SEGYLOG_GSM_DEBUG(message) \
    segysdk::logger::Logger::getInstance().logGSM((segysdk::logger::LogLevel::Debug), (message))
#define SEGYLOG_GSM_INFO(message) \
    segysdk::logger::Logger::getInstance().logGSM((segysdk::logger::LogLevel::Info), (message))
#define SEGYLOG_GSM_WARNING(message) \
    segysdk::logger::Logger::getInstance().logGSM((segysdk::logger::LogLevel::Warning), (message))
#define SEGYLOG_GSM_ERROR(message) \
    segysdk::logger::Logger::getInstance().logGSM((segysdk::logger::LogLevel::Error), (message))

// Special handling for errors when should be reported to all loggers
#define SEGYLOG_REPORT_ERROR(message) \
    {                                 \
        SEGYLOG_ERROR(message);       \
        SEGYLOG_GSM_ERROR(message);   \
    }

#define SEGYLOG_REPORT_WARNING(message) \
    {                                   \
        SEGYLOG_WARNING(message);       \
        SEGYLOG_GSM_ERROR(message);     \
    }

#define SEGYLOG_REPORT_INFO(message) \
    {                                \
        SEGYLOG_INFO(message);       \
        SEGYLOG_GSM_INFO(message);   \
    }

#endif

#else
#define SEGYLOG_REPORT_ERROR(args) (void)(args)
#define SEGYLOG_REPORT_WARNING(args) (void)(args)
#endif
