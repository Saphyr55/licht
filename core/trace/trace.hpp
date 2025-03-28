#pragma once

#include "core/defines.hpp"
#include "core/trace/logger.hpp"

#define LMAKE_CONTEXT_LOG_MSG(severity, channel, msg) \
    ::licht::LogMessage(severity, channel, msg, uint32(__LINE__), __FUNCTION__, __FILE__, false)

#define LLOG(severity, channel, msg) \
    ::licht::Logger::get_default().log(LMAKE_CONTEXT_LOG_MSG(severity, channel, msg));

#define LLOG_DEBUG(channel, msg) \
    LLOG(::licht::LogSeverity::DEBUG, channel, msg)

#define LLOG_INFO(channel, msg) \
    LLOG(::licht::LogSeverity::INFO, channel, msg)

#define LLOG_WARN(channel, msg) \
    LLOG(::licht::LogSeverity::WARN, channel, msg)

#define LLOG_ERROR(channel, msg) \
    LLOG(licht::LogSeverity::ERROR, channel, msg)

#define LLOG_FATAL(channel, msg)                     \
    LLOG(::licht::LogSeverity::FATAL, channel, msg) \
    LDEBUGBREAK();

#define LLOG_INFO_WHEN(cond, channel, msg) \
    if ((cond)) {                         \
        LLOG_INFO(channel, msg)            \
    }

#define LLOG_FATAL_WHEN(cond, channel, msg) \
    if ((cond)) {                          \
        LLOG_FATAL(channel, msg)            \
    }

#define LLOG_ERROR_WHEN(cond, channel, msg) \
    if ((cond)) {                          \
        LLOG_ERROR(channel, msg)            \
    }

#define LLOG_WARN_WHEN(cond, channel, msg) \
    if ((cond)) {                         \
        LLOG_WARN(channel, msg)            \
    }

#define LLOG_DEBUG_WHEN(cond, channel, msg) \
    if ((cond)) {                          \
        LLOG_DEBUG(channel, msg)            \
    }
