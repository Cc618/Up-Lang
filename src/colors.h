#pragma once

// Definitions to use colors

#include <string>

// Colors for one variable
#define AS_BLUE(s) (BLUE + s + DEFAULT)
#define AS_RED(s) (RED + s + DEFAULT)
#define AS_GREEN(s) (GREEN + s + DEFAULT)
#define AS_YELLOW(s) (YELLOW + s + DEFAULT)

#define AS_BLUE_S(s) BLUE << (s) << DEFAULT
#define AS_RED_S(s) RED << (s) << DEFAULT
#define AS_GREEN_S(s) GREEN << (s) << DEFAULT
#define AS_YELLOW_S(s) YELLOW << (s) << DEFAULT

namespace up
{
    static const std::string DEFAULT = "\e[0m";
    static const std::string RED = "\e[31m";
    static const std::string GREEN = "\e[32m";
    static const std::string YELLOW = "\e[33m";
    static const std::string BLUE = "\e[34m";
} // namespace up
