#pragma once
#include <string>
#include <vector>

enum class LogType {
    INFO,
    WARNING,
    ERROR
};

struct LogEntry {
    LogType type;
    std::string message;
};

class Logger {
public:
    static std::vector<LogEntry> messages;
    static void Log(const std::string& message);
    static void Error(const std::string& message);

};
