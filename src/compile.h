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

/* `path` is the real path of the file `content` came from - even though
   `content` is formatted purely in memory, cake still needs `path` to
   resolve #include "quoted.h" headers relative to the right directory
   (angle-bracket <system.h> headers don't need it; those come from
   cakeconf.h's #pragma dir entries instead). Pass NULL/"" when there is no
   real file (e.g. an unsaved buffer) - quoted includes just won't resolve.

   To restrict the fix to a line range (e.g. an editor's current selection),
   put -format-lines=first:last in pszoptions - same flag as the CLI (see
   options.c), one parsing path instead of a second one just for this. */
const char* _Owner _Opt cake_format(const char* pszoptions, const char* _Opt path, const char* content, struct report* report);

/*
   This function is called by the web version
*/   
char* _Owner _Opt CompileText(const char* pszoptions, const char* content);
void print_report(const struct report* report);