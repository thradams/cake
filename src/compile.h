/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
 *
 *  struct object is used to compute the compile time expressions (including constexpr)
 *
*/

#pragma once
#include "ownership.h"
#include <stdbool.h>


struct report;
int compile(int argc, const char** argv, struct report* report);
const char* _Owner _Opt compile_source(const char* pszoptions, const char* content, struct report* report);

/*
   This function is called by the web version
*/   
char* _Owner _Opt CompileText(const char* pszoptions, const char* content);
