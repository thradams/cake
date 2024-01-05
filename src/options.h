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
    W_NONE = 0,  /*not a real warning, used in especial cases*/
    W_UNUSED_VARIABLE = 1 << 1, //-Wunused-variable
    W_DEPRECATED = 1 << 2,
    W_ENUN_CONVERSION = 1 << 3 ,//-Wenum-conversion
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
    W_DISCARDED_QUALIFIERS = 1 << 15,
    W_DECLARATOR_STATE = 1 << 16,
    W_UNINITIALZED  = 1 << 17,
    
    W_RETURN_LOCAL_ADDR = 1 << 20,
    
};

enum error
{ 
    C_SUCCESS = 0,
    C_UNEXPECTED,    
    C_TOO_MANY_ARGUMENTS,
    C_TOO_FEW_ARGUMENTS,
    C_NOT_FOUND,
    C_NO_MATCH_FOR_GENERIC,
    C_SUBSCRIPTED_VALUE_IS_NEITHER_ARRAY_NOR_POINTER,
    C_CALLED_OBJECT_IS_NOT_FUNCTION_OR_FUNCTION_POINTER,
    C_STRUCT_MEMBER_NOT_FOUND,
    C_STRUCTURE_OR_UNION_REQUIRED,
    C_STRUCT_IS_INCOMPLETE,
    C_DECLARATOR_NOT_FOUND,
    C_EXPECTED_DECLARATOR_NAME,
    C_UNKNOWN_ATTRIBUTE_NAME,
    C_INDIRECTION_REQUIRES_POINTER_OPERAND,
    C_INVALID_TOKEN,
    C_EXPECTED_STRUCT_TYPE,
    C_EXPECTED_TYPE_NAME,
    C_LEFT_IS_NOT_ARITHMETIC,
    C_RIGHT_IS_NOT_ARITHMETIC,
    C_DIVIZION_BY_ZERO,
    C_LEFT_IS_NOT_INTEGER,
    C_RIGHT_IS_NOT_INTEGER,
    C_INVALID_TYPE,
    C_LEFT_IS_NOT_SCALAR,
    C_RIGHT_IS_NOT_SCALAR,
    C_INCOMPATIBLE_POINTER_TYPES,
    C_ASSIGNMENT_OF_FUNCTION,
    C_ASSIGNMENT_TO_EXPRESSION_WITH_ARRAY_TYPE,
    C_ASSIGNMENT_OF_READ_ONLY_OBJECT,
    C_LVALUE_ASSIGNMENT,
    C_CONDITION_MUST_HAVE_SCALAR_TYPE,
    C_INCOMPATIBLE_TYPES,
    C_EXPECTED_CONSTANT_EXPRESSION,
    C_UNEXPECTED_TOKEN,
    C_CANNOT_COMBINE_WITH_PREVIOUS_LONG_LONG,
    C_EXPECTED_DECLARATION,
    C_STATIC_OR_TYPE_QUALIFIERS_NOT_ALLOWED_IN_NON_PARAMETER,
    C_OBJ_OWNER_CAN_BE_USED_ONLY_IN_POINTER,
    C_REDECLARATION,
    C_TAG_TYPE_DOES_NOT_MATCH_PREVIOUS_DECLARATION,
    C_MISSING_ENUM_TAG_NAME,
    C_MULTIPLE_DEFINITION_ENUM,
    C_STATIC_ASSERT_FAILED,
    C_ATTR_UNBALANCED,    
    C_UNEXPECTED_END_OF_FILE,
    C_THROW_STATEMENT_NOT_WITHIN_TRY_BLOCK,
    C_VOID_FUNCTION_SHOULD_NOT_RETURN_VALUE,
    C_ARGUMENT_SIZE_SMALLER_THAN_PARAMETER_SIZE,
    C_PASSING_NULL_AS_ARRAY,
    C_DISCARDING_CONST_AT_ARGUMENT,
    C_INCOMPATIBLE_ENUN_TYPES,
    C_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS,
    C_FILE_NOT_FOUND,
    C_MISSING_CLOSE_PARENTHESIS,
    C_EXPRESSION_ERROR,
    C_PREPROCESSOR_ERROR_DIRECTIVE,
    C_TOO_FEW_ARGUMENTS_TO_FUNCTION_LIKE_MACRO,
    C_MACRO_INVALID_ARG,
    C_MISSING_MACRO_ARGUMENT,
    C_ADDRESS_OF_REGISTER,
    C_OPERATOR_NEEDS_LVALUE, //C2105
    C_MULTICHAR_ERROR,
    C_CHARACTER_TOO_LARGE,
    C_OWNERSHIP_NON_OWNER_MOVE,
    C_PRAGMA_ERROR,
    /*ownership type system errors*/
    C_OWNERSHIP_MISSING_OWNER_QUALIFIER,
    C_OWNERSHIP_NOT_OWNER,
    C_OWNERSHIP_USING_TEMPORARY_OWNER,
    C_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
    
    C_OWNERSHIP_NON_OWNER_TO_OWNER_ASSIGN,
    C_DISCARDING_OWNER,
    
    /*flow analysis errors*/
    C_OWNERSHIP_FLOW_MISSING_DTOR,    
};

bool is_ownership_error(enum error e);

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
       Causes the compiler to output a list of the include files.
       The option also displays nested include files, that is, 
       the files included by the files that you include.
    */
    bool show_includes;

    /*
       -remove-comments  
    */
    bool remove_comments;    

    /*
       -flow-analysis
    */
    bool flow_analysis;

    /*
      -E 
    */
    bool preprocess_only;

    bool disable_ownership_errors;
    /*
      -rm
      -direct-compilation
    */
    bool direct_compilation;

    /*
      -sarif
    */
    bool sarif_output;

    bool format_input;
    bool format_ouput;
    
    
    /*
      -no-output
      if true cake does not generate ouput
    */
    bool no_output;
    
    /* 
    -nullchecks
     
    */
    bool null_checks;

    /*
      -msvc-output
    */
    bool visual_studio_ouput_format;

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
