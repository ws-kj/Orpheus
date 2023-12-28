#pragma once

#include <iostream>
#include <stdexcept>

class ErrorHandler {
public:
    static bool had_error;

    static void Error(int line, const std::string& msg);
    static void RuntimeError(const std::string& msg);
    static void Report(int line, const std::string& msg);
};
