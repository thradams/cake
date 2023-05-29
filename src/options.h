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

enum warning {    
    W_UNUSED_VARIABLE = 1 << 1, //-Wunused-variable
    W_DEPRECATED = 1 << 2,
    W_ENUN_COMPARE = 1 << 3 ,//-Wenum-compare
    W_NON_NULL = 1 << 4, //-Wnonnull
    W_ADDRESS = 1 << 5, //-Waddress (always true)
    W_UNUSED_PARAMETER = 1 << 6, //-Wno-unused-parameter
    W_DECLARATOR_HIDE = 1 << 7, // gcc no
    W_TYPEOF_ARRAY_PARAMETER = 1 << 8,//
    W_ATTRIBUTES = 1 << 9, //-Wattributes
    W_UNUSED_VALUE = 1 << 10, //-Wunused-value
    W_STYLE = 1 << 11, //-Wstyle
    W_COMMENT = 1 << 12,
    W_LINE_SLICING = 1 << 13,
    W_STRING_SLICED = 1 << 14,
};

enum style
{
    STYLE_CAKE,

    // https://llvm.org/docs/CodingStandards.html
    STYLE_LLVM,// A style complying with the LLVM coding standards
    
    //https://google.github.io/styleguide/cppguide.html
    STYLE_GOOGLE,// A style complying with Google’s C++ style guide

    //https://chromium.googlesource.com/chromium/src/+/refs/heads/main/styleguide/styleguide.md
    //https://www.kernel.org/doc/html/latest/process/coding-style.html
    STYLE_CHROMIUM,// A style complying with Chromium’s style guide
    
    //https://firefox-source-docs.mozilla.org/code-quality/coding-style/index.html
    STYLE_MOZILLA,// A style complying with Mozilla’s style guide
    
    //https://www.webkit.org/code-style-guidelines/
    STYLE_WEBKIT,// A style complying with WebKit’s style guide
    
    STYLE_MICROSOFT,// A style complying with Microsoft’s style guide
    
    STYLE_GNU,// A style complying with the GNU coding standards
    
};
const char* get_warning_name(enum warning w);
enum warning  get_warning_flag(const char* wname);

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
      #pragma CAKE diagnostic push
      #pragma CAKE diagnostic pop
    */
    int enabled_warnings_stack_top_index;
    enum warning enabled_warnings_stack[10];

    enum style style;

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


    
    /*
      -no-output
      if true cake does not generate ouput
    */
    bool no_output;
    
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
