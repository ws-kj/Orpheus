#include <iostream>
#include <fstream>
#include <sstream>

#include "../include/error_handler.h"
#include "../include/scanner.h"

void run(const std::string& source) {
    Scanner scanner = Scanner(source);

    std::vector<Token> tokens = scanner.ScanTokens();
    for(const auto& t : tokens) {
        std::cout << t << "\n";
    }
}

int run_file(char *filename) {
    std::ifstream t(filename);

    if(!t) {
        std::cout << "Error: could not open " << filename << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << t.rdbuf();

    run(buffer.str());


    if(ErrorHandler::had_error) {
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    if(argc != 2) {
        std::cout << "Usage: orpheus [file.or]\n";
        return 1;
    }

    return run_file(argv[1]);

    return 0;
}
