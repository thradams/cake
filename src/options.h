/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#include <stdbool.h>
#include "target.h"
#include <limits.h>

enum language_version
{
    LANGUAGE_C23,
    LANGUAGE_C2Y,
    LANGUAGE_CAK,
};

enum diagnostic_id {

    W_LOCATION = 0,

    W_WARNING_DIRECTIVE = 1,
    W_UNUSED_VARIABLE = 2,
    W_DEPRECATED = 3,
    W_ENUN_CONVERSION = 4,

    W_ADDRESS = 5,
    W_UNUSED_PARAMETER = 6,
    W_DECLARATOR_HIDE = 7,
    W_TYPEOF_ARRAY_PARAMETER = 8,
    W_ATTRIBUTES = 9,
    W_EXPRESSION_RESULT_NOT_USED = 10,
    W_STYLE = 11,
    W_MULTI_LINE_COMMENT = 12,
    W_LINE_SLICING = 13,
    W_STRING_SLICED = 14,
    W_DISCARDED_QUALIFIERS = 15,
    W_UNUSED_WARNING_16 = 16,
    W_UNINITIALZED = 17,
    W_RETURN_LOCAL_ADDR = 18,
    W_MUST_USE_ADDRESSOF = 19,
    W_ARRAY_INDIRECTION = 20,

    W_UNUSED_WARNING_21 = 21,
    W_OWNERSHIP_NOT_OWNER = 22,
    W_OWNERSHIP_USING_TEMPORARY_OWNER = 23,
    W_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER = 24,
    W_OWNERSHIP_NON_OWNER_TO_OWNER_ASSIGN = 25,
    W_OWNERSHIP_DISCARDING_OWNER = 26,
    W_OWNERSHIP_NON_OWNER_MOVE = 27,

    W_FLOW_NON_NULL = 28,
    W_FLOW_MISSING_DTOR = 29,
    W_FLOW_UNINITIALIZED = 30,
    W_FLOW_LIFETIME_ENDED = 31,
    W_FLOW_MOVED = 32,
    W_FLOW_NULL_DEREFERENCE = 33,
    W_FLOW_NOT_USED_34 = 34,
    W_FLOW_NULLABLE_TO_NON_NULLABLE = 35,
    W_FLOW_DIVIZION_BY_ZERO = 36,


    W_DIVIZION_BY_ZERO = 37,
    W_UNUSED_WARNING_38 = 38,
    W_PASSING_NULL_AS_ARRAY = 39,
    W_INCOMPATIBLE_ENUN_TYPES = 40,
    W_MULTICHAR_ERROR = 41,
    W_OUT_OF_BOUNDS = 42,
    W_ASSIGNMENT_OF_ARRAY_PARAMETER = 43,
    W_CONDITIONAL_IS_CONSTANT = 44,
    W_SWITCH = 45,
    W_UNSUAL_NULL_POINTER_CONSTANT = 46,
    W_SIZEOF_ARRAY_ARGUMENT = 47,
    W_CONST_NOT_INITIALIZED = 48,
    W_NULL_CONVERTION = 49,
    W_IMPLICITLY_UNSIGNED_LITERAL = 50,
    W_INTEGER_OVERFLOW = 51,
    W_ARRAY_SIZE = 52,


    W_EMPTY_STATEMENT = 53,
    W_ERROR_INCOMPATIBLE_TYPES = 54,
    W_UNUSED_LABEL = 55,
    W_REDEFINING_BUITIN_MACRO = 56,
    W_UNUSED_FUNCTION = 57,
    W_BOOL_COMPARISON = 58,
    W_WARNING_DID_NOT_HAPPEN = 59,
    W_NULLABLE_TO_NON_NULLABLE = 60,
    W_CAST_TO_SAME_TYPE = 61,

    W_TO_MANY_INITIALIZERS = 62,
    W_FLOAT_RANGE = 63,

    
    W_UNUSED_WARNING_64 = 64,
    W_UNUSED_WARNING_65 = 65,
    W_UNUSED_WARNING_66 = 66,
    W_UNUSED_WARNING_67 = 67,
    W_UNUSED_WARNING_68 = 68,
    W_UNUSED_WARNING_69 = 69,
    W_UNUSED_WARNING_70 = 70,
    W_UNUSED_WARNING_71 = 71,
    W_UNUSED_WARNING_72 = 72,
    W_UNUSED_WARNING_73 = 73,
    W_UNUSED_WARNING_74 = 74,
    W_UNUSED_WARNING_75 = 75,
    W_UNUSED_WARNING_76 = 76,
    W_UNUSED_WARNING_77 = 77,
    W_UNUSED_WARNING_78 = 78,
    W_UNUSED_WARNING_79 = 79,
    W_UNUSED_WARNING_80 = 80,
    W_UNUSED_WARNING_81 = 81,
    W_UNUSED_WARNING_82 = 82,
    W_UNUSED_WARNING_83 = 83,
    W_UNUSED_WARNING_84 = 84,
    W_UNUSED_WARNING_85 = 85,
    W_UNUSED_WARNING_86 = 86,
    W_UNUSED_WARNING_87 = 87,
    W_UNUSED_WARNING_88 = 88,
    W_UNUSED_WARNING_89 = 89,
    W_UNUSED_WARNING_90 = 90,
    W_UNUSED_WARNING_91 = 91,
    W_UNUSED_WARNING_92 = 92,
    W_UNUSED_WARNING_93 = 93,
    W_UNUSED_WARNING_94 = 94,
    W_UNUSED_WARNING_95 = 95,
    W_UNUSED_WARNING_96 = 96,
    W_UNUSED_WARNING_97 = 97,
    W_UNUSED_WARNING_98 = 98,
    W_UNUSED_WARNING_99 = 99,
    W_UNUSED_WARNING_100 = 100,
    W_UNUSED_WARNING_101 = 101,
    W_UNUSED_WARNING_102 = 102,
    W_UNUSED_WARNING_103 = 103,
    W_UNUSED_WARNING_104 = 104,
    W_UNUSED_WARNING_105 = 105,
    W_UNUSED_WARNING_106 = 106,
    W_UNUSED_WARNING_107 = 107,
    W_UNUSED_WARNING_108 = 108,
    W_UNUSED_WARNING_109 = 109,
    W_UNUSED_WARNING_110 = 110,
    W_UNUSED_WARNING_111 = 111,
    W_UNUSED_WARNING_112 = 112,
    W_UNUSED_WARNING_113 = 113,
    W_UNUSED_WARNING_114 = 114,
    W_UNUSED_WARNING_115 = 115,
    W_UNUSED_WARNING_116 = 116,
    W_UNUSED_WARNING_117 = 117,
    W_UNUSED_WARNING_118 = 118,
    W_UNUSED_WARNING_119 = 119,
    W_UNUSED_WARNING_120 = 120,
    W_UNUSED_WARNING_121 = 121,
    W_UNUSED_WARNING_122 = 122,
    W_UNUSED_WARNING_123 = 123,
    W_UNUSED_WARNING_124 = 124,
    W_UNUSED_WARNING_125 = 125,
    W_UNUSED_WARNING_126 = 126,
    W_UNUSED_WARNING_127 = 127,
    

    
    

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
    C_ERROR_MACRO_REDEFINITION = 1820,
    C_ERROR_INVALID_PREPROCESSING_DIRECTIVE = 1830,
    C_ERROR_FUNCTION_CANNOT_BE_MEMBER = 1840,
    C_ERROR_NON_INTEGRAL_ENUM_TYPE = 1850,
    C_ERROR_REQUIRES_COMPILE_TIME_VALUE = 1860,
    C_ERROR_OUTER_SCOPE = 1870,
};


bool is_diagnostic_configurable(enum diagnostic_id id);



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

#define BITSET_SIZE 128
#define BITSET_WORD_BITS (CHAR_BIT * sizeof(unsigned long))
#define BITSET_WORDS ((BITSET_SIZE + BITSET_WORD_BITS - 1) / BITSET_WORD_BITS)

struct bitset
{
    unsigned long bits[BITSET_WORDS];
};


struct diagnostic
{

    /*set of warnings reported as errors*/
    struct bitset errors;

    /*set of warnings reported as warnings*/
    struct bitset warnings;

    /*set of warnings reported as notes*/
    struct bitset notes;
};

int get_diagnostic_type(struct diagnostic* d, enum diagnostic_id w);
extern struct diagnostic default_diagnostic;

void diagnostic_remove(struct diagnostic* d, enum diagnostic_id w);

struct diagnostic_stack
{
    int top_index;
    struct diagnostic stack[10];
};

int diagnostic_stack_push_empty(struct diagnostic_stack* diagnostic_stack);
void diagnostic_stack_pop(struct diagnostic_stack* diagnostic_stack);

struct diagnostic_id_stack
{
    int size;
    enum  diagnostic_id stack[10];
};

int diagnostic_id_stack_push(struct diagnostic_id_stack* diagnostic_stack, enum diagnostic_id id);
void diagnostic_id_stack_pop(struct diagnostic_id_stack* diagnostic_stack);


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
      -test-in-out
    */
    bool test_mode_inout;

    /*
    * -nullchecks
    */
    bool null_checks_enabled;

    bool ownership_enabled;

    /*
      -E
    */
    bool preprocess_only;



    /*
      -preprocess-def-macro
    */
    bool preprocess_def_macro;

    bool clear_error_at_end; //used by tests

    /*
      -sarif
    */
    bool sarif_output;

    /*
      -no-output
      if true cake does not generate output
    */
    bool no_output;

    /*
     -const-literal
     makes literal strings const
    */
    bool const_literal;

    /*
      -fdiagnostics-format=msvc
      -msvc-output
    */
    bool visual_studio_ouput_format;

    /*
      -fdiagnostics-color=never
    */
    bool color_disabled;

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
       -comment-to-attr
    */
    bool comment_to_attribute;

    bool do_static_debug;
    int static_debug_lines;

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

bool is_diagnostic_enabled(const struct options* options, enum diagnostic_id w);

void print_help();

void options_set_error(struct options* options, enum diagnostic_id w, bool value);
void options_set_warning(struct options* options, enum diagnostic_id w, bool value);
void options_set_note(struct options* options, enum diagnostic_id w, bool value);
void options_set_all_warnings(struct options* options);
void options_set_clear_all_warnings(struct options* options);

bool options_diagnostic_is_error(const struct options* options, enum diagnostic_id w);
bool options_diagnostic_is_warning(const struct options* options, enum diagnostic_id w);
bool options_diagnostic_is_note(const struct options* options, enum diagnostic_id w);