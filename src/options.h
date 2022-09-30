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
    bool bRemoveComments;
    bool bPreprocessOnly;
    bool bRemoveMacros;
    bool format_input;
    bool format_ouput;
    bool nodiscard_is_default;

    /*
    * true - to info about name convensions violations
    */
    bool check_naming_conventions;
};
