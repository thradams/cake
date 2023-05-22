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
    /*
       -std=c99
    */
    enum language_version input;

    /*
       -target=c99
    */
    enum language_version target;
    
    /*
       -remove-comments  
    */
    bool remove_comments;    

    /*
      -E 
    */
    bool preprocess_only;

    /*
      -rm
      -direct-compilation
    */
    bool direct_compilation;

    bool format_input;
    bool format_ouput;
    
    /*
      -no-discard 
    */
    bool nodiscard_is_default;


    bool do_static_analisys;

    /*
      -no-output
      if true cake does not generate ouput
    */
    bool no_output;
    
    /*
      -n
      true - to info about name conventions violations
    */
    bool check_naming_conventions;

    /*
      -o filename
      defines the ouputfile when 1 file is used
    */
    char output[200];
};

int fill_options(struct options* options,
                 int argc,
                 const char** argv);

void print_help();
