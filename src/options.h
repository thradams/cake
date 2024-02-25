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

    W_MUST_USE_ADDRESSOF,
    /*ownership type system errors*/
    W_OWNERSHIP_MISSING_OWNER_QUALIFIER,
    W_OWNERSHIP_NOT_OWNER,
    W_OWNERSHIP_USING_TEMPORARY_OWNER,
    W_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,

    W_OWNERSHIP_NON_OWNER_TO_OWNER_ASSIGN,
    W_DISCARDING_OWNER,
    W_OWNERSHIP_NON_OWNER_MOVE,

    /*flow analysis errors*/
    W_ANALYZER_OWNERSHIP_FLOW_MISSING_DTOR,


    W_ANALYZER_UNINITIALIZED,
    W_ANALYZER_NULL_DEREFERENCE,

    W_DIVIZION_BY_ZERO,
    W_PASSING_NULL_AS_ARRAY,
    W_ANALIZER_MAYBE_NULL_TO_NON_OPT_ARGUMENT,
    W_INCOMPATIBLE_ENUN_TYPES,
    W_MULTICHAR_ERROR,
    W_LOCATION,
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
    ERROR_NOT_FOUND,
    ERROR_NO_MATCH_FOR_GENERIC,
    ERROR_SUBSCRIPTED_VALUE_IS_NEITHER_ARRAY_NOR_POINTER,
    ERROR_CALLED_OBJECT_IS_NOT_FUNCTION_OR_FUNCTION_POINTER,
    ERROR_STRUCT_MEMBER_NOT_FOUND,
    ERROR_STRUCTURE_OR_UNION_REQUIRED,
    ERROR_STRUCT_IS_INCOMPLETE,
    ERROR_DECLARATOR_NOT_FOUND,
    ERROR_EXPECTED_DECLARATOR_NAME,
    ERROR_UNKNOWN_ATTRIBUTE_NAME,
    ERROR_INDIRECTION_REQUIRES_POINTER_OPERAND,
    ERROR_INVALID_TOKEN,
    ERROR_EXPECTED_STRUCT_TYPE,
    ERROR_EXPECTED_TYPE_NAME,
    ERROR_LEFT_IS_NOT_ARITHMETIC,
    ERROR_RIGHT_IS_NOT_ARITHMETIC,
    
    ERROR_LEFT_IS_NOT_INTEGER,
    ERROR_RIGHT_IS_NOT_INTEGER,
    ERROR_INVALID_TYPE,
    ERROR_LEFT_IS_NOT_SCALAR,
    ERROR_RIGHT_IS_NOT_SCALAR,
    ERROR_INCOMPATIBLE_POINTER_TYPES, //warning?
    ERROR_ASSIGNMENT_OF_FUNCTION,
    ERROR_ASSIGNMENT_TO_EXPRESSION_WITH_ARRAY_TYPE,
    ERROR_ASSIGNMENT_OF_READ_ONLY_OBJECT,
    ERROR_LVALUE_ASSIGNMENT,
    ERROR_CONDITION_MUST_HAVE_SCALAR_TYPE,
    ERROR_INCOMPATIBLE_TYPES,
    ERROR_EXPECTED_CONSTANT_EXPRESSION,
    ERROR_UNEXPECTED_TOKEN,
    ERROR_CANNOT_COMBINE_WITH_PREVIOUS_LONG_LONG,
    ERROR_EXPECTED_DECLARATION,
    ERROR_STATIC_OR_TYPE_QUALIFIERS_NOT_ALLOWED_IN_NON_PARAMETER,
    ERROR_OBJ_OWNER_CAN_BE_USED_ONLY_IN_POINTER,
    ERROR_REDECLARATION,
    ERROR_TAG_TYPE_DOES_NOT_MATCH_PREVIOUS_DECLARATION,
    ERROR_MISSING_ENUM_TAG_NAME,
    ERROR_MULTIPLE_DEFINITION_ENUM,
    ERROR_STATIC_ASSERT_FAILED,
    ERROR_STATIC_STATE_FAILED,
    ERROR_ATTR_UNBALANCED,
    ERROR_UNEXPECTED_END_OF_FILE,
    ERROR_THROW_STATEMENT_NOT_WITHIN_TRY_BLOCK,
    ERROR_VOID_FUNCTION_SHOULD_NOT_RETURN_VALUE,
    ERROR_ARGUMENT_SIZE_SMALLER_THAN_PARAMETER_SIZE,

    C_DISCARDING_CONST_AT_ARGUMENT, //warning?
      
    ERROR_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS,
    ERROR_FILE_NOT_FOUND,
    ERROR_MISSING_CLOSE_PARENTHESIS,
    ERROR_EXPRESSION_ERROR,
    ERROR_PREPROCESSOR_ERROR_DIRECTIVE,
    ERROR_TOO_FEW_ARGUMENTS_TO_FUNCTION_LIKE_MACRO,
    ERROR_PREPROCESSOR_MACRO_INVALID_ARG,
    ERROR_PREPROCESSOR_MISSING_MACRO_ARGUMENT,
    ERROR_ADDRESS_OF_REGISTER,
    ERROR_OPERATOR_NEEDS_LVALUE, //C2105
    
    ERROR_CHARACTER_TOO_LARGE,

    ERROR_PRAGMA_ERROR,
    ERROR_OUT_OF_MEM,

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

enum diagnostic_id  get_warning(const char* wname);

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
int get_diagnostic_type(struct diagnostic* d, enum diagnostic_id w);
extern struct diagnostic default_diagnostic;

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

    bool clear_error_at_end; //used by tests
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
      print tokens before preprocessor
    */
    bool dump_tokens;

    /*
      -dump-pp-tokens
      (print tokens after preprocessor)
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
