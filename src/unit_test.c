/*do not edit this file*/

#include <stdio.h>

#ifdef TEST
#define TESTCODE
#endif
int g_unit_test_error_count = 0;
int g_unit_test_success_count = 0;
#ifdef TESTCODE

/*forward declarations*/

/* tests from token.c*/
void token_list_remove_get_test(void);
void token_list_remove_get_test2(void);

/* tests from tokenizer.c*/
void test_lexeme_cmp(void);
void token_list_pop_front_test(void);
void token_list_pop_back_test(void);
int token_list_append_list_test(void);
void test_collect(void);
void test_va_opt_0(void);
void test_va_opt_1(void);
void test_va_opt_2(void);
void test_va_opt_3(void);
void test_va_opt_4(void);
void test_va_opt_5(void);
void test_va_opt_6(void);
void test_va_opt_7(void);
void concatenation_problem(void);
void test_va_opt_G2(void);
void test_va_opt(void);
void test_empty_va_args(void);
void test_va_args_single(void);
void test_va_args_extra_args(void);
void test_empty_va_args_empty(void);
void test_defined(void);
void testline(void);
void ifelse(void);
void T1(void);
int EXAMPLE5(void);
void recursivetest1(void);
void rectest(void);
void emptycall(void);
void semiempty(void);
void calling_one_arg_with_empty_arg(void);
void test_argument_with_parentesis(void);
void two_empty_arguments(void);
void simple_object_macro(void);
void test_one_file(void);
void test2(void);
void test3(void);
void tetris(void);
void recursive_macro_expansion(void);
void empty_and_no_args(void);
void empty_and_args(void);
void test4(void);
void test_string(void);
void test6(void);
void testerror(void);
int test_expression(void);
int test_concatenation_o(void);
int test_concatenation(void);
void bad_test(void);
int test_spaces(void);
int test_stringfy(void);
int test_tokens(void);
int test_predefined_macros(void);
int test_utf8(void);
int bug_test(void);
int test_line_continuation(void);
int stringify_test(void);

/* tests from options.c*/
void test_get_warning_name(void);

/*end of forward declarations*/

int test_main(void)
{
g_unit_test_error_count = 0;
g_unit_test_success_count = 0;
    token_list_remove_get_test();
    token_list_remove_get_test2();
    test_lexeme_cmp();
    token_list_pop_front_test();
    token_list_pop_back_test();
    token_list_append_list_test();
    test_collect();
    test_va_opt_0();
    test_va_opt_1();
    test_va_opt_2();
    test_va_opt_3();
    test_va_opt_4();
    test_va_opt_5();
    test_va_opt_6();
    test_va_opt_7();
    concatenation_problem();
    test_va_opt_G2();
    test_va_opt();
    test_empty_va_args();
    test_va_args_single();
    test_va_args_extra_args();
    test_empty_va_args_empty();
    test_defined();
    testline();
    ifelse();
    T1();
    EXAMPLE5();
    recursivetest1();
    rectest();
    emptycall();
    semiempty();
    calling_one_arg_with_empty_arg();
    test_argument_with_parentesis();
    two_empty_arguments();
    simple_object_macro();
    test_one_file();
    test2();
    test3();
    tetris();
    recursive_macro_expansion();
    empty_and_no_args();
    empty_and_args();
    test4();
    test_string();
    test6();
    testerror();
    test_expression();
    test_concatenation_o();
    test_concatenation();
    bad_test();
    test_spaces();
    test_stringfy();
    test_tokens();
    test_predefined_macros();
    test_utf8();
    bug_test();
    test_line_continuation();
    stringify_test();
    test_get_warning_name();
return g_unit_test_error_count;

}
#undef TESTCODE
#endif /*TEST*/
