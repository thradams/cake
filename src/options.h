#pragma once
#include <stdbool.h>

/*
* Compiler options shared with compiler and preprocessor
*/

enum LanguageVersion
{
    LANGUAGE_C99 = 0, /*default*/
    LANGUAGE_C11,
    LANGUAGE_C2X, //C23
    LANGUAGE_CXX, //experimental
};

struct options
{
    enum LanguageVersion input;
    enum LanguageVersion target;
    bool bRemoveComments;
    bool bPreprocessOnly;
    bool bRemoveMacros;
};
