#pragma once
#include <stdbool.h>

/*
* Compiler options shared with compiler and preprocessor
*/

enum language_version
{
    LANGUAGE_C99 = 0, /*default*/
    LANGUAGE_C11,
    LANGUAGE_C2X, //C23
    LANGUAGE_CXX, //experimental
};

struct options
{
    enum language_version input;
    enum language_version target;
    bool bRemoveComments;
    bool bPreprocessOnly;
    bool bRemoveMacros;
    
    /*
    * true - to info about name convensions violations
    */
    bool check_naming_conventions;
};
