/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#include <stdbool.h>

/*
* Compiler options shared with compiler and preprocessor
*/

enum target
{
    TARGET_NONE = -1,
    TARGET_C89_IL = 0 /*default*/
};

enum language_version
{
    LANGUAGE_C23,
    LANGUAGE_C2Y,
    LANGUAGE_CAK,
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
    //////////////////////////////////////////////
    W_FLOW_NON_NULL, //-Wnonnull
    W_FLOW_MISSING_DTOR,
    W_FLOW_UNINITIALIZED,
    W_FLOW_LIFETIME_ENDED,
    W_FLOW_MOVED,
    W_FLOW_NULL_DEREFERENCE,
    W_FLOW_MAYBE_NULL_TO_NON_OPT_ARG,
    W_FLOW_NULLABLE_TO_NON_NULLABLE,
    W_FLOW_DIVIZION_BY_ZERO,    
    //////////////////////////////////////////////
    W_DIVIZION_BY_ZERO,
    W_CONSTANT_VALUE, /*sample 0 * a */
    W_PASSING_NULL_AS_ARRAY,
    W_INCOMPATIBLE_ENUN_TYPES,
    W_MULTICHAR_ERROR,
    W_OUT_OF_BOUNDS,
    W_ASSIGNMENT_OF_ARRAY_PARAMETER,
    W_CONDITIONAL_IS_CONSTANT,
    W_SWITCH,
    W_UNSUAL_NULL_POINTER_CONSTANT,
    W_SIZEOF_ARRAY_ARGUMENT,
    W_CONST_NOT_INITIALIZED,
    W_NULL_CONVERTION,
    W_IMPLICITLY_UNSIGNED_LITERAL,
    W_INTEGER_OVERFLOW,
    W_ARRAY_SIZE,
    
    
    W_EMPTY_STATEMENT,
    W_ERROR_INCOMPATIBLE_TYPES,
    W_UNUSED_LABEL,
    W_REDEFINING_BUITIN_MACRO,
    W_UNUSED_FUNCTION,
    W_NOT_DEFINED57,
    W_NOT_DEFINED58,
    W_NOT_DEFINED59,
    W_NOT_DEFINED60,
    W_NOT_DEFINED61,

    W_LOCATION, /*prints code location*/
    W_NOTE,

    //----------------------------------------------------------------
    
    W_TO_MANY_INITIALIZERS = 100,

    //---------------------------------------------------------------

    C_ERROR_INVALID_QUALIFIER_FOR_POINTER = 640,
    C_ERROR_UNEXPECTED = 650,
    C_ERROR_TOO_MANY_ARGUMENTS = 660,
    C_ERROR_TOO_FEW_ARGUMENTS = 670,
    C_ERROR_NOT_FOUND = 680,
    C_ERROR_NO_MATCH_FOR_GENERIC = 690,
    C_ERROR_SUBSCRIPTED_VALUE_IS_NEITHER_ARRAY_NOR_POINTER = 700,
    C_ERROR_CALLED_OBJECT_IS_NOT_FUNCTION_OR_FUNCTION_POINTER = 710,
    C_ERROR_STRUCT_MEMBER_NOT_FOUND = 720,
    C_ERROR_STRUCTURE_OR_UNION_REQUIRED = 730,
    C_ERROR_STRUCT_IS_INCOMPLETE = 740,
    C_ERROR_DECLARATOR_NOT_FOUND = 750,
    C_ERROR_EXPECTED_DECLARATOR_NAME = 760,
    C_ERROR_UNKNOWN_ATTRIBUTE_NAME = 770,
    C_ERROR_INDIRECTION_REQUIRES_POINTER_OPERAND = 780,
    C_ERROR_INVALID_TOKEN = 790,
    C_ERROR_EXPECTED_STRUCT_TYPE = 800,
    C_ERROR_EXPECTED_TYPE_NAME = 810,
    C_ERROR_LEFT_IS_NOT_ARITHMETIC = 820,
    C_ERROR_RIGHT_IS_NOT_ARITHMETIC = 830,
    C_ERROR_LEFT_IS_NOT_INTEGER = 840,
    C_ERROR_RIGHT_IS_NOT_INTEGER = 850,
    C_ERROR_INVALID_TYPE = 860,
    C_ERROR_LEFT_IS_NOT_SCALAR = 870,
    C_ERROR_RIGHT_IS_NOT_SCALAR = 880,
    C_ERROR_INCOMPATIBLE_POINTER_TYPES = 890, //warning?
    C_ERROR_ASSIGNMENT_OF_FUNCTION = 900,
    C_ERROR_ASSIGNMENT_TO_EXPRESSION_WITH_ARRAY_TYPE = 910,
    C_ERROR_ASSIGNMENT_OF_READ_ONLY_OBJECT = 920,
    C_ERROR_LVALUE_ASSIGNMENT = 930,
    C_ERROR_CONDITION_MUST_HAVE_SCALAR_TYPE = 940,
    C_ERROR_INCOMPATIBLE_TYPES = 950,
    C_ERROR_EXPECTED_CONSTANT_EXPRESSION = 960,
    C_ERROR_UNEXPECTED_TOKEN = 970,
    C_ERROR_CANNOT_COMBINE_WITH_PREVIOUS_LONG_LONG = 980,
    C_ERROR_EXPECTED_DECLARATION = 990,
    C_ERROR_STATIC_OR_TYPE_QUALIFIERS_NOT_ALLOWED_IN_NON_PARAMETER = 1000,
    C_ERROR_OBJ_OWNER_CAN_BE_USED_ONLY_IN_POINTER = 1010,
    C_ERROR_REDECLARATION = 1020,
    C_ERROR_TAG_TYPE_DOES_NOT_MATCH_PREVIOUS_DECLARATION = 1030,
    C_ERROR_MISSING_ENUM_TAG_NAME = 1040,
    C_ERROR_MULTIPLE_DEFINITION_ENUM = 1050,
    C_ERROR_STATIC_ASSERT_FAILED = 1060,
    C_ERROR_STATIC_SET = 1070,
    C_ANALIZER_ERROR_STATIC_STATE_FAILED = 1080,
    C_ERROR_ATTR_UNBALANCED = 1090,
    C_ERROR_UNEXPECTED_END_OF_FILE = 1100,
    C_ERROR_THROW_STATEMENT_NOT_WITHIN_TRY_BLOCK = 1110,
    C_ERROR_VOID_FUNCTION_SHOULD_NOT_RETURN_VALUE = 1120,
    C_ERROR_NON_VOID_FUNCTION_SHOULD_RETURN_VALUE = 1121,
    C_ERROR_ARGUMENT_SIZE_SMALLER_THAN_PARAMETER_SIZE = 1130,
    C_ERROR_TOKEN_NOT_VALID_IN_PREPROCESSOR_EXPRESSIONS = 1140,
    C_ERROR_FILE_NOT_FOUND = 1150,
    C_ERROR_MISSING_CLOSE_PARENTHESIS = 1160,
    C_ERROR_EXPRESSION_ERROR = 1170,
    C_ERROR_PREPROCESSOR_C_ERROR_DIRECTIVE = 1180,
    C_ERROR_TOO_FEW_ARGUMENTS_TO_FUNCTION_LIKE_MACRO = 1190,
    C_ERROR_TOO_MANY_ARGUMENTS_TO_FUNCTION_LIKE_MACRO = 1191,
    C_ERROR_PREPROCESSOR_MACRO_INVALID_ARG = 1200,
    C_ERROR_PREPROCESSOR_MISSING_MACRO_ARGUMENT = 1210,
    C_ERROR_ADDRESS_OF_REGISTER = 1220,
    C_ERROR_OPERATOR_NEEDS_LVALUE = 1230, //C2105
    C_ERROR_CHARACTER_TOO_LARGE = 1240,
    C_ERROR_PRAGMA_ERROR = 1250,
    C_ERROR_OUT_OF_MEM = 1260,
    C_ERROR_STORAGE_SIZE = 1270,
    C_ERROR_RETURN_LOCAL_OWNER_TO_NON_OWNER = 1280,
    C_ERROR_AUTO_NEEDS_SINGLE_DECLARATOR = 1290,
    C_ERROR_TWO_OR_MORE_SPECIFIERS = 1300,
    C_ERROR_OPERATOR_INCREMENT_CANNOT_BE_USED_IN_OWNER = 1310,
    C_ERROR_OPERATOR_DECREMENT_CANNOT_BE_USED_IN_OWNER = 1320,
    C_PRE_DIVISION_BY_ZERO = 1330,
    C_ERROR_INT_TO_POINTER = 1340,
    C_ERROR_LITERAL_OVERFLOW = 1350,
    C_CHARACTER_NOT_ENCODABLE_IN_A_SINGLE_CODE_UNIT = 1360,
    C_MULTICHAR_ERROR = 1370,
    C_INVALID_TOKEN = 1380,
    C_INVALID_ARGUMENT_NELEMENTSOF = 1390,
    C_ERROR_RETURN_CANNOT_BE_USED_INSIDE_DEFER = 1400,
    C_ERROR_FUNCTION_RETURNS_FUNCTION = 1410,
    C_ERROR_FUNCTION_RETURNS_ARRAY = 1420,    
    C_ERROR_LABEL_NOT_DEFINED = 1430,    
    C_ERROR_DUPLICATED_LABEL = 1440,
    C_ERROR_DUPLICATED_CASE = 1450,
    C_ERROR_SUBSCRIPT_IS_NOT_AN_INTEGER = 1560,    
    C_ERROR_DUPLICATE_DEFAULT_GENERIC_ASSOCIATION = 1570, 
    C_ERROR_MULTIPLE_DEFAULT_LABELS_IN_ONE_SWITCH = 1780,
    C_ERROR_POINTER_TO_FLOATING_TYPE = 1790,
    C_ERROR_FLOATING_TYPE_TO_POINTER = 1800,
    C_ERROR_NULLPTR_CAST_ERROR = 1810,
};


bool is_diagnostic_configurable(enum diagnostic_id id);
bool is_diagnostic_warning(enum diagnostic_id id);
bool is_diagnostic_error(enum diagnostic_id id);
bool is_diagnostic_note(enum diagnostic_id id);


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

    /*set of warnings reported as errors*/
    unsigned long long errors;
    /*set of warnings reported as warnings*/
    unsigned long long warnings;
    /*set of warnings reported as notes*/
    unsigned long long notes;
};

int get_diagnostic_type(struct diagnostic* d, enum diagnostic_id w);
extern struct diagnostic default_diagnostic;

void diagnostic_remove(struct diagnostic *d, enum diagnostic_id w);

struct diagnostic_stack
{
    int top_index;
    struct diagnostic stack[10];
};

int diagnostic_stack_push_empty(struct diagnostic_stack* diagnostic_stack);
void diagnostic_stack_pop(struct diagnostic_stack* diagnostic_stack);


struct options
{
    enum language_version input;
    enum target target;

    /*
      #pragma CAKE diagnostic push
      #pragma CAKE diagnostic pop
    */
    struct diagnostic_stack diagnostic_stack;

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
      -sarif
    */
    bool sarif_output;

    /*
      -no-output
      if true cake does not generate ouput
    */
    bool no_output;

    /*
     -def-preprocess
    */
    bool def_macro_preprocess;

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
    char sarifpath[200];
};

int fill_options(struct options* options,
                 int argc,
                 const char** argv);

void print_help();

