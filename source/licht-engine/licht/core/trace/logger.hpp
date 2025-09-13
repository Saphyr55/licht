#pragma once

#include <cstdint>
#include <functional>

#include "licht/core/containers/array.hpp"
#include "licht/core/core_exports.hpp"
#include "licht/core/string/string_ref.hpp"

namespace licht {

/**
 * @enum LogSeverity
 * @brief Defines the severity levels for log messages.
 */
enum class LICHT_CORE_API LogSeverity {
    Info,
    Warn,
    Debug,
    Error,
    Fatal,
};

/**
 * @struct LogMessage
 * @brief Represents a log message with associated metadata.
 */
struct LICHT_CORE_API LogMessage {
    /** Severity level of the log message. */
    LogSeverity severity = LogSeverity::Debug;
    
    /** Channel or category of the log message. */
    StringRef channel = "UNKNOWN";
    
    /** The actual log message. */
    StringRef message = "NO MESSAGE";

    /** Line number where the log message was generated. */
    uint32 line = UINT32_MAX;
    
    /** Function name where the log message was generated. */
    StringRef function = "GLOBAL";
    
    /** File name where the log message was generated. */
    StringRef file = "GLOBAL";
    
    /** Flag indicating whether the log message is verbose. */
    bool verbose = false;

    LogMessage() = default;
    LogMessage(LogSeverity p_severity,
               StringRef p_channel,
               StringRef p_message,
               uint32 p_line,
               StringRef p_function,
               StringRef p_file,
               bool p_verbose)
        : severity(p_severity)
        , channel(p_channel)
        , message(p_message)
        , line(p_line)
        , function(p_function)
        , file(p_file)
        , verbose(p_verbose) {}

    ~LogMessage() = default;
};

/**
 * @typedef LogFn
 * @brief Function type for logging callbacks.
 */
using LogFn = std::function<void(const LogMessage& message)>;

/**
 * @class Logger
 * @brief Provides a logging interface with a delegate function.
 */
class LICHT_CORE_API Logger {
public:
    /**
     * @brief Gets the default logger instance.
     * @return Reference to the default logger.
     */
    static Logger& get_default();

    /**
     * @brief Logs a message using the registered delegate.
     * @param message The log message to be logged.
     */
    void log(const LogMessage& message) const;

    /**
     * @brief Gets the current logging delegate.
     * @return The current logging function.
     */
    LogFn get_delegate() const;

    /**
     * @brief Sets the logging delegate.
     * @param func The logging function to set as the delegate.
     */
    void set_delegate(LogFn func);

    /**
     * @brief Constructs a Logger with a specified logging function.
     * @param fn The logging function to use as the delegate.
     */
    Logger(LogFn fn);

    /**
     * @brief Destructor.
     */
    ~Logger() = default;

private:
    /** The logging delegate function. */
    LogFn log_fn_;
};

/**
 * @class ConsoleLogger
 * @brief Provides a logger that outputs messages to the console.
 */
class LICHT_CORE_API ConsoleLogger {
public:
    /**
     * @brief Gets the logging delegate for console output.
     * @return The logging function for console output.
     */
    LogFn get_delegate();

    /**
     * @brief Constructs a ConsoleLogger.
     */
    ConsoleLogger();

    /**
     * @brief Destructor.
     */
    ~ConsoleLogger() = default;

private:
    /** The logging function for console output. */
    LogFn console_log_fn_; 
};

/**
 * @class CompositeLogger
 * @brief Combines multiple loggers into a single logger.
 */
class LICHT_CORE_API CompositeLogger {
public:
    /**
     * @brief Adds a logging function to the composite logger.
     * @param logger The logging function to add.
     */
    void add_log_fn(LogFn logger);

    /**
     * @brief Logs a message using all registered logging functions.
     * @param message The log message to be logged.
     */
    void log(const LogMessage& message) const;

private:
    /** Collection of registered logging functions. */
    Array<LogFn> loggers_;
};

}  //namespace licht