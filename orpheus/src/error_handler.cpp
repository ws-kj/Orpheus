#include "../include/error_handler.h"

bool ErrorHandler::had_error = false;

void ErrorHandler::Error(int line, const std::string& msg) {
    Report(line, msg);
}

void ErrorHandler::RuntimeError(const std::string& msg) {
    Error(-1, msg);
}

void ErrorHandler::Report(int line, const std::string& msg) {
    if(line != -1) {
        std::cout << "Error on line " << std::to_string(line) << ": " << msg << "\n";       
    }
    had_error = true;
}
