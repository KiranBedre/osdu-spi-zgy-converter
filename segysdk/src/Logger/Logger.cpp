/*
Copyright 2022 Schlumberger

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

#include "../Logger/Logger.h"
#include "../LibraryLoader/DynamicLibraryLoader.h"
#include "../Logger/LogWriterOstream.h"
#include "../Logger/Message.h"
#include "../Logger/MessageGSM.h"
#include "../Utilities/Environment.h"

#include "../Utilities/SafeTime.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <cstring>
#include "Logger.h"

namespace segysdk::logger
{

LogLevel Logger::level_ = LogLevel::Debug;

void
Logger::setLogLevel(LogLevel level)
{
    level_ = level;
}

const char*
toString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Debug:
        return "LogLevel::Debug";
        break;
    case LogLevel::Info:
        return "LogLevel::Info";
        break;
    case LogLevel::Warning:
        return "LogLevel::Warning";
        break;
    case LogLevel::Error:
        return "LogLevel::Error";
        break;
    default:
        break;
    }
    throw std::runtime_error("Unknown LogLevel");
}

bool
Logger::isLowLogLevel(LogLevel level)
{
    return static_cast<int>(level) < static_cast<int>(Logger::level_);
}

Logger::Logger()
  : logPool_(std::make_shared<MessagePool>()),
    dispatcher_(logPool_)
{
    LogLevel level = (segysdk::utilities::Environment::instance().indexingVerbosity() < 1
                      && segysdk::utilities::Environment::instance().segyToZgyVerbosity() < 1)
                         ? LogLevel::Info
                         : LogLevel::Debug;
    setLogLevel(level);
}

Logger::~Logger()
{
    stopLogWriterDispatcher();
}

void
Logger::logGSM(LogLevel level, const char* message)
{
    logGSM(level, std::string(message));
}

void
Logger::logGSM(LogLevel level, const std::stringstream& message)
{
    logGSM(level, message.str());
}

void
Logger::logGSM(LogLevel level, std::string message)
{
    MessageGSM m(message);
    logGSM(level, m);
}

Logger&
Logger::getInstance()
{
    static Logger log_{};
    return log_;
}

void
Logger::addLogWriterPlugin(std::shared_ptr<DynamicLibraryLoader> plugin, const LoggerSettings& settings)
{
    // Check that the plugin provides sufficient functions to create/delete LogWriter
    Logger::createLogWriter creater = (Logger::createLogWriter)plugin->getFunction(CreateLogWriterFunctionName_);
    Logger::deleteLogWriter deleter = (Logger::deleteLogWriter)plugin->getFunction(DeleteLogWriterFunctionName_);
    if (creater != nullptr && deleter != nullptr) {
        LogWriterBase* writer = creater(&settings);
        if (writer != nullptr) {
            std::shared_ptr<LogWriterBase> plugin_writer(writer, deleter);
            addLogWriterToDispatcher(plugin_writer);
            logWriterPlugins_.emplace_back(plugin);
        }
    }
    else {
        std::string err_msg = "Loaded plugin: " + plugin->getLibraryPath() + " doesn't have required functions to crete/delete LogWriter";
        SEGYLOG(LogLevel::Warning, err_msg);
    }
}

Logger&
initializeLogger(const LoggerSettings& settings)
{
    auto& log = Logger::getInstance();
    auto plugin_path = settings.getLogWriterPluginPath();
    if (!plugin_path.empty())
    {
       auto plugin = std::make_shared<Logger::DynamicLibraryLoader>(plugin_path.string());
       log.addLogWriterPlugin(plugin, settings);
    }
    // Print all logged messages to cout
    log.addLogWriterToDispatcher(std::make_shared<LogWriterOstream>(std::make_shared<std::ostream>(std::cout.rdbuf())));

    log.runLogWriterDispatcher();
    return log;
}

void
Logger::addLogWriterToDispatcher(std::shared_ptr<LogWriterBase> writer)
{
    dispatcher_.addLogWriter(writer);
}

void
Logger::runLogWriterDispatcher()
{
    dispatcher_.runDispatcher();
}

void
Logger::stopLogWriterDispatcher()
{
    dispatcher_.stopDispatcher();
}

void
Logger::log(LogLevel level, const char* func, const char* file, int line, int argc, const char** argv)
{
    std::string message{"Command line arguments:"};
    for (auto i = 0; i < argc; ++i) {
        message = message + " [" + argv[i] + "]";
    }
    log(level, func, file, line, message);
}

void
Logger::log(LogLevel level, const char* func, const char* file, int line, const std::stringstream& sstream)
{
    if (static_cast<int>(level) < static_cast<int>(level_)) {
        return;
    }
    std::chrono::system_clock::time_point tp_now = std::chrono::system_clock::now();
    std::time_t now = std::chrono::system_clock::to_time_t(tp_now);
    struct tm tm_now {};
    segysdk::utilities::safe_gmtime(now, tm_now);
    char time_buffer[512] = {};
    const char* time_format = "%Y-%m-%d %H:%M:%S";
    std::strftime(time_buffer, sizeof(time_buffer), time_format, &tm_now);

    const std::chrono::duration<double> tse = tp_now.time_since_epoch();
    std::chrono::seconds::rep milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(tse).count() % 1000;

    std::stringstream out_stream;
    out_stream << time_buffer << "." << std::setfill('0') << std::setw(3) << milliseconds << " ";
    out_stream << "(" << std::this_thread::get_id() << ")"
               << " ";
    out_stream << "[" << toString(level) << "]"
               << " ";
    out_stream << "[" << std::filesystem::path(file).filename() << ":" << line << ":" << func << "]"
               << ": ";
    out_stream << sstream.rdbuf();
    out_stream << std::endl;
    logPool_->add(std::make_unique<Message>(out_stream.str()));
}

} // namespace segysdk::logger
