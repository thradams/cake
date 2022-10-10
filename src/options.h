#pragma once
#include <stdbool.h>

/*
* Compiler options shared with compiler and preprocessor
*/

enum language_version
{
    LANGUAGE_C89 = -1,
    LANGUAGE_C99 = 0, /*default*/
    LANGUAGE_C11 = 1,
    LANGUAGE_C2X = 2, //C23
    LANGUAGE_CXX = 3, //experimental
};

struct options
{
    enum language_version input;
    enum language_version target;
    bool remove_comments;
    bool preprocess_only;
    bool remove_macros;
    bool format_input;
    bool format_ouput;
    bool nodiscard_is_default;
    bool do_static_analisys;
    /*
    * true - to info about name convensions violations
    */
    bool check_naming_conventions;
};
