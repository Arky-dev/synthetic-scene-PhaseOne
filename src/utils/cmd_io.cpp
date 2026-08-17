#include "cmd_io.hpp"
#include <iostream>

int static read_int(const std::string& prompt, int default_val) {
    std::cout << prompt << " [Default: " << default_val << "]: ";
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) return default_val;
    try {
        return std::stoi(line);
    }
    catch (...) {
        return default_val;
    }
}

float static read_float(const std::string& prompt, float default_val) {
    std::cout << prompt << " [Default: " << default_val << "]: ";
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) return default_val;
    try {
        return std::stoi(line);
    }
    catch (...) {
        return default_val;
    }
}

std::string static read_string(const std::string& prompt, const std::string& default_val) {
    std::cout << prompt << " [Default: " << default_val << "]: ";
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) return default_val;
    return line;
}