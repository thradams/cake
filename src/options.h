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
    W_ARRAY_INDIRECTION,
    

    /*ownership type system errors*/
    W_OWNERSHIP_MISSING_OWNER_QUALIFIER,
    W_OWNERSHIP_NOT_OWNER,
    W_OWNERSHIP_USING_TEMPORARY_OWNER,
    W_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
    W_OWNERSHIP_NON_OWNER_TO_OWNER_ASSIGN,
    W_OWNERSHIP_DISCARDING_OWNER,
    W_OWNERSHIP_NON_OWNER_MOVE,
    
   
    /*
      Flow analysis warning
    */
    W_FLOW_NON_NULL, //-Wnonnull
    W_FLOW_MISSING_DTOR,
    W_FLOW_UNINITIALIZED,
    W_FLOW_LIFETIME_ENDED,
    W_FLOW_MOVED,
    W_FLOW_NULL_DEREFERENCE,
    W_FLOW_MAYBE_NULL_TO_NON_OPT_ARG,
    W_FLOW_NULLABLE_TO_NON_NULLABLE,
    /**/

    W_DIVIZION_BY_ZERO,
    W_PASSING_NULL_AS_ARRAY,
    
    W_INCOMPATIBLE_ENUN_TYPES,
    W_MULTICHAR_ERROR,   
    W_OUT_OF_BOUNDS,
    W_ASSIGNMENT_OF_ARRAY_PARAMETER,
    W_CONDITIONAL_IS_CONSTANT,
    
    W_SWITCH,
    W_UNSUAL_NULL_POINTER_CONSTANT,
    W_SIZEOF_ARRAY_ARGUMENT,
    W_NOT_DEFINED44,
    W_NOT_DEFINED45,
    W_NOT_DEFINED46,
    W_NOT_DEFINED47,
    W_NOT_DEFINED48,
    W_NOT_DEFINED49,
    W_NOT_DEFINED50,
    W_NOT_DEFINED51,
    W_NOT_DEFINED52,
    W_NOT_DEFINED53,
    W_NOT_DEFINED54,
    W_NOT_DEFINED55,
    W_NOT_DEFINED56,
    W_NOT_DEFINED57,
    W_NOT_DEFINED58,
    W_NOT_DEFINED59,
    W_NOT_DEFINED60,
    W_NOT_DEFINED61,

    W_LOCATION, /*prints code location*/
    W_NOTE,

    //AFTER THIS POINT (W_NOTE) MESSAGES ARE ALWAYS ERRORS
    ////////////////////////////////////////

    //constraints violations are errors
    C_ERROR_UNEXPECTED,
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
    C_ERROR_TOO_MANY_ARGUMENTS,
    C_ERROR_TOO_FEW_ARGUMENTS,
    
    //TODO.... more constrains
    C_ERROR_NOT_FOUND,
    C_ERROR_NO_MATCH_FOR_GENERIC,
    C_ERROR_SUBSCRIPTED_VALUE_IS_NEITHER_ARRAY_NOR_POINTER,
    C_ERROR_CALLED_OBJECT_IS_NOT_FUNCTION_OR_FUNCTION_POINTER,
    C_ERROR_STRUCT_MEMBER_NOT_FOUND,
    C_ERROR_STRUCTURE_OR_UNION_REQUIRED,
    C_ERROR_STRUCT_IS_INCOMPLETE,
    C_ERROR_DECLARATOR_NOT_FOUND,
    C_ERROR_EXPECTED_DECLARATOR_NAME,
    C_ERROR_UNKNOWN_ATTRIBUTE_NAME,
    C_ERROR_INDIRECTION_REQUIRES_POINTER_OPERAND,
    C_ERROR_INVALID_TOKEN,
    C_ERROR_EXPECTED_STRUCT_TYPE,
    C_ERROR_EXPECTED_TYPE_NAME,
    C_ERROR_LEFT_IS_NOT_ARITHMETIC,
    C_ERROR_RIGHT_IS_NOT_ARITHMETIC,
    
    C_ERROR_LEFT_IS_NOT_INTEGER,
    C_ERROR_RIGHT_IS_NOT_INTEGER,
    C_ERROR_INVALID_TYPE,
    C_ERROR_LEFT_IS_NOT_SCALAR,
    C_ERROR_RIGHT_IS_NOT_SCALAR,
    C_ERROR_INCOMPATIBLE_POINTER_TYPES, //warning?
    C_ERROR_ASSIGNMENT_OF_FUNCTION,
    C_ERROR_ASSIGNMENT_TO_EXPRESSION_WITH_ARRAY_TYPE,
    C_ERROR_ASSIGNMENT_OF_READ_ONLY_OBJECT,
    C_ERROR_LVALUE_ASSIGNMENT,
    C_ERROR_CONDITION_MUST_HAVE_SCALAR_TYPE,
    C_ERROR_INCOMPATIBLE_TYPES,
    C_ERROR_EXPECTED_CONSTANT_EXPRESSION,
    C_ERROR_UNEXPECTED_TOKEN,
    C_ERROR_CANNOT_COMBINE_WITH_PREVIOUS_LONG_LONG,    
    C_ERROR_EXPECTED_DECLARATION,
    C_ERROR_STATIC_OR_TYPE_QUALIFIERS_NOT_ALLOWED_IN_NON_PARAMETER,
    C_ERROR_OBJ_OWNER_CAN_BE_USED_ONLY_IN_POINTER,
    C_ERROR_REDECLARATION,
    C_ERROR_TAG_TYPE_DOES_NOT_MATCH_PREVIOUS_DECLARATION,
    C_ERROR_MISSING_ENUM_TAG_NAME,
    C_ERROR_MULTIPLE_DEFINITION_ENUM,
    C_ERROR_STATIC_ASSERT_FAILED,
    C_ERROR_STATIC_SET,
    C_ANALIZER_ERROR_STATIC_STATE_FAILED,
    C_ERROR_ATTR_UNBALANCED,
    C_ERROR_UNEXPECTED_END_OF_FILE,
    C_ERROR_THROW_STATEMENT_NOT_WITHIN_TRY_BLOCK,
    C_ERROR_VOID_FUNCTION_SHOULD_NOT_RETURN_VALUE,
    C_ERROR_ARGUMENT_SIZE_SMALLER_THAN_PARAMETER_SIZE,
 
    
    C_ERROR_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS,
    C_ERROR_FILE_NOT_FOUND,
    C_ERROR_MISSING_CLOSE_PARENTHESIS,
    C_ERROR_EXPRESSION_ERROR,
    C_ERROR_PREPROCESSOR_C_ERROR_DIRECTIVE,
    C_ERROR_TOO_FEW_ARGUMENTS_TO_FUNCTION_LIKE_MACRO,
    C_ERROR_PREPROCESSOR_MACRO_INVALID_ARG,
    C_ERROR_PREPROCESSOR_MISSING_MACRO_ARGUMENT,
    C_ERROR_ADDRESS_OF_REGISTER,
    C_ERROR_OPERATOR_NEEDS_LVALUE, //C2105
    
    C_ERROR_CHARACTER_TOO_LARGE,

    C_ERROR_PRAGMA_ERROR,
    C_ERROR_OUT_OF_MEM,
    C_ERROR_STORAGE_SIZE,
    C_ERROR_RETURN_LOCAL_OWNER_TO_NON_OWNER,
    C_ERROR_AUTO_NEEDS_SINGLE_DECLARATOR,    
    C_ERROR_TWO_OR_MORE_SPECIFIERS,
    C_ERROR_OPERATOR_INCREMENT_CANNOT_BE_USED_IN_OWNER,
    C_ERROR_OPERATOR_DECREMENT_CANNOT_BE_USED_IN_OWNER,
    C_PRE_DIVISION_BY_ZERO,
    C_ERROR_INT_TO_POINTER
};

/*
* These warnings are removed when "nullable=disable"
*/
#define WFLAG(W) (1ULL << W)
#define NULLABLE_DISABLE_REMOVED_WARNINGS  (WFLAG(W_FLOW_NULL_DEREFERENCE) | WFLAG(W_FLOW_NULLABLE_TO_NON_NULLABLE))

#define OWNERSHIP_DISABLE_REMOVED_WARNINGS  (WFLAG(W_FLOW_UNINITIALIZED))


int get_diagnostic_phase(enum diagnostic_id w);

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
int get_warning_name(enum diagnostic_id w, int n, char buffer[/*n*/]);
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
       -disable-assert
    */
    bool disable_assert;


    /*
       -remove-comments
    */
    bool remove_comments;

    /*
       -flow-analysis
    */
    bool flow_analysis;

    /*
    * -testmode
    */
    bool test_mode;

    /*
    * -nullchecks
    */
    bool null_checks_enabled;

    bool ownership_enabled;

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
      -fdiagnostics-format=msvc
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
      -autoconfig      
    */
    bool auto_config;

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

