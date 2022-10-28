#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>

std::vector<LogEntry> Logger::messages;
std::string CurrentDatetimeToString() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char output[30];
    std::strftime(output, sizeof(output), "%d-%b-%Y %H:%M:%S", std::localtime(&now));
    return output;
}

void Logger::Log(const std::string& message) {
    LogEntry logEntry = {LogType::INFO, "LOG: [" + CurrentDatetimeToString() + "] " + message};
    std::cout << "\x1b[32m" << logEntry.message << "\033[0m" << "\n";
    messages.emplace_back(logEntry);
}

void Logger::Error(const std::string& message) {
    LogEntry logEntry = {LogType::ERROR, "ERR: [" + CurrentDatetimeToString() + "] " + message};
    std::cout << "\x1b[91m" << logEntry.message << "\033[0m" << "\n";
    messages.emplace_back(logEntry);
}
