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

enum diagnostic_id {
    W_NONE = 0,  /*not a real warning, used in especial cases*/
    W_UNUSED_VARIABLE, //-Wunused-variable
    W_DEPRECATED,
    W_ENUN_CONVERSION,//-Wenum-conversion
    W_NON_NULL, //-Wnonnull
    W_ADDRESS, //-Waddress (always true)
    W_UNUSED_PARAMETER, //-Wno-unused-parameter
    W_DECLARATOR_HIDE, // gcc no
    W_TYPEOF_ARRAY_PARAMETER,//
    W_ATTRIBUTES, //-Wattributes
    W_UNUSED_VALUE, //-Wunused-value
    W_STYLE, //-Wstyle
    W_COMMENT,
    W_LINE_SLICING,
    W_STRING_SLICED,
    W_DISCARDED_QUALIFIERS,
    W_DECLARATOR_STATE,
    W_UNINITIALZED,

    W_RETURN_LOCAL_ADDR,


    /*ownership type system errors*/
    C_OWNERSHIP_MISSING_OWNER_QUALIFIER,
    C_OWNERSHIP_NOT_OWNER,
    C_OWNERSHIP_USING_TEMPORARY_OWNER,
    C_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,

    C_OWNERSHIP_NON_OWNER_TO_OWNER_ASSIGN,
    C_DISCARDING_OWNER,

    /*flow analysis errors*/
    C_OWNERSHIP_FLOW_MISSING_DTOR,
    C_OWNERSHIP_NON_OWNER_MOVE,

    C_MAYBE_UNINITIALIZED,
    C_NULL_DEREFERENCE,

    W_NOTE,
    //AFTER THIS POINT (W_NOTE) MESSAGES ARE ALWAYS ERRORS
    ////////////////////////////////////////

    //constraints violations are errors
    C_UNEXPECTED,
    //TODO ERROR MESSAGE MUST BE BASED ON CONSTRAINS
    // 
    
    /*
    * 5.1.1.3 Diagnostics
      A conforming implementation shall produce at least one diagnostic message (identified in an
      implementation-defined manner) if a preprocessing translation unit or translation unit contains a
      violation of any syntax rule or constraint, even if the behavior is also explicitly specified as undefined
      or implementation-defined. Diagnostic messages need not be produced in other circumstances.9
    */

    //SAMPLE
    //Constrain : The number of arguments shall agree with the number of parameters.
    ERROR_TOO_MANY_ARGUMENTS,
    ERROR_TOO_FEW_ARGUMENTS,
    
    //TODO.... more constrains
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

    C_PRAGMA_ERROR,

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
const char* get_warning_name(enum diagnostic_id w);
unsigned long long  get_warning_bit_mask(const char* wname);

struct diagnostic
{
    /*
      each message has number (0-63) that corresponds to the bit index
      Messages bigger than W_NOTE are errors or bigger than 63
    */

    /*set of diagnostics reported as errors*/
    unsigned long long errors;
    /*set of diagnostics reported as warnings*/
    unsigned long long warnings;
    /*set of diagnostics reported as notes*/
    unsigned long long notes;
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
      #pragma CAKE diagnostic push
      #pragma CAKE diagnostic pop
    */
    int diagnostic_stack_top_index;
    struct diagnostic diagnostic_stack[10];

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
      -dump-tokens
    */
    bool dump_tokens;

    /*
      -dump-pp-tokens
    */
    bool dump_pptokens;

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
