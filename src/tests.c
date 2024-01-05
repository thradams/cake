
#include <string.h>

#ifdef TEST

#include "parser.h"
#include <stdlib.h>

#include "unit_test.h"

static bool compile_without_errors(bool flow_analysis, bool nullchecks, const char* src)
{
    struct options options = {.input = LANGUAGE_C99, .flow_analysis = flow_analysis, .null_checks = nullchecks};
    struct report report = {0};
    get_ast(&options, "source", src, &report);
    return report.error_count == 0;
}

static bool compile_with_errors(bool flow_analysis, bool nullchecks, const char* src)
{

    struct options options = {.input = LANGUAGE_C99, .flow_analysis = flow_analysis, .null_checks = nullchecks};
    struct report report = {0};
    get_ast(&options, "source", src, &report);
    return report.error_count != 0;
}

void parser_specifier_test()
{
    const char* src = "long long long i;";
    assert(compile_with_errors(false, false, src));
}

void char_constants()
{
    const char* source
        =
        "#define TYPE_IS(e, T) _Generic(typeof(e), T : 1, default: 0)\n"
        "\n"
        "static_assert(U'ç' == 231);\n"
        "static_assert(u'ç' == 231);\n"
        "//static_assert('ç' == 231);\n"
        "\n"
        "static_assert(TYPE_IS('a', int));\n"
        "static_assert(TYPE_IS(u8'a', unsigned char));\n"
        "static_assert(TYPE_IS(u'a', unsigned short));\n"
        "static_assert(TYPE_IS(U'a', unsigned int));";

    assert(compile_without_errors(false, false, source));
}

void array_item_type_test()
{
    const char* src =
        "#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)\n"
        "void (*pf[10])(void* val);\n"
        "static_assert(_is_same(typeof(pf[0]), void (*)(void* val)));\n";
    assert(compile_without_errors(false, false, src));
}

void take_address_type_test()
{
    const char* src =
        "void F(char(*p)[10])"
        "{"
        "    (*p)[0] = 'a';"
        "}";
    assert(compile_without_errors(false, false, src));
}

void parser_scope_test()
{
    const char* src = "void f() {int i; char i;}";
    assert(compile_with_errors(false, false, src));
}

void parser_tag_test()
{
    //mudou tipo do tag no mesmo escopo
    const char* src = "enum E { A }; struct E { int i; };";
    assert(compile_with_errors(false, false, src));
}

void string_concatenation_test()
{
    const char* src = " const char* s = \"part1\" \"part2\";";
    assert(compile_without_errors(false, false, src));
}

void test_digit_separator()
{
    struct report report = {0};
    char* result = compile_source("-std=c99", "int i = 1'000;", &report);
    assert(strcmp(result, "int i = 1000;") == 0);
    free(result);
}

void test_lit()
{
    struct report report = {0};
    char* result = compile_source("-std=c99", "char * s = u8\"maçã\";", &report);
    assert(strcmp(result, "char * s = \"ma\\xc3\\xa7\\xc3\\xa3\";") == 0);
    free(result);
}

void type_test2()
{
    char* src =
        "#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)\n"
        "int a[10];\n"
        " static_assert(_is_same(typeof(&a) ,int (*)[10]));\n"
        ;

    assert(compile_without_errors(false, false, src));
}

void type_test3()
{
    char* src =
        "#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)\n"
        "int i;"
        "int (*f)(void);"
        " static_assert(_is_same(typeof(&i), int *));"
        " static_assert(_is_same(typeof(&f), int (**)(void)));"
        ;

    assert(compile_without_errors(false, false, src));
}

void crazy_decl()
{
    const char* src =
        "void (*f(int i))(void)\n"
        "{\n"
        "   i = 1; \n"
        "    return 0;\n"
        "}\n";

    assert(compile_without_errors(false, false, src));
}

void crazy_decl2()
{
    const char* src =
        "void (*f(int i))(void)\n"
        "{\n"
        "   i = 1; \n"
        "    return 0;\n"
        "}\n"
        "int main()\n"
        "{\n"
        "  f(1);\n"
        "}\n";

    assert(compile_without_errors(false, false, src));
}

void crazy_decl4()
{
    const char* src =
        "void (*F(int a, int b))(void) { return 0; }\n"
        "void (*(*PF)(int a, int b))(void) = F;\n"
        "int main() {\n"
        "    PF(1, 2);\n"
        "}\n";

    assert(compile_without_errors(false, false, src));
}

void sizeof_not_evaluated()
{
    assert(compile_without_errors(false, false, "int i = sizeof(1/0);\n"));
}

void sizeof_array_test()
{
    assert(compile_without_errors(false,
        false,
        "int main() {\n"
        "int a[] = { 1, 2, 3 };\n"
        "static_assert(sizeof(a) == sizeof(int) * 3);\n"
        "}\n"
    ));
}

void sizeof_test()
{

    const char* src =
        "static_assert(sizeof(\"ABC\") == 4);"
        "char a[10];"
        "char b[10][2];"
        "static_assert(sizeof(a) == 10);"
        "static_assert(sizeof(b) == sizeof(char)*10*2);"
        "char *p[10];"
        "static_assert(sizeof(p) == 40);"
        "static_assert(sizeof(int) == 4);"
        "static_assert(sizeof(long) == 4);"
        "static_assert(sizeof(char) == 1);"
        "static_assert(sizeof(short) == 4);"
        "static_assert(sizeof(unsigned int) == 4);"
        "static_assert(sizeof(void (*pf)(int i)) == sizeof(void*));"
        ;

    assert(compile_without_errors(false, false, src));
}

void alignof_test()
{
    const char* src =
        "struct X { char s; double c; char s2;};\n"
        "static_assert(alignof(struct X) == 8);"
        "static_assert(sizeof(struct X) == 24);"
        ;

    assert(compile_without_errors(false, false, src));
}

void indirection_struct_size()
{
    const char* src =
        "typedef struct X X;\n"
        "struct X {\n"
        "    void* data;\n"
        "};\n"
        "static_assert(sizeof(X) == sizeof(void*));"
        ;

    assert(compile_without_errors(false, false, src));
}

void traits_test()
{
    //https://en.cppreference.com/w/cpp/header/type_traits
    const char* src =
        "void (*F)();\n"
        "static_assert(_is_pointer(F));\n"
        "static_assert(_is_integral(1));\n"
        "int a[2];\n"
        "static_assert(_is_array(a));\n"
        "int((a2))[10];\n"
        "static_assert(_is_array(a2));"
        ;
    assert(compile_without_errors(false, false, src));
}

void comp_error1()
{
    const char* src =
        "void F() {\n"
        "    char* z;\n"
        "    *z-- = '\\0';\n"
        "}\n";

    assert(compile_without_errors(false, false, src));
}

void array_size()
{
    const char* src =
        "void (*f[2][3])(int i);\n"
        "int main() {\n"
        "static_assert(sizeof(void (*[2])(int i)) == sizeof(void*) * 2);\n"
        "static_assert(sizeof(f) == sizeof(void (*[2])(int i)) * 3);\n"
        "}"
        ;

    assert(compile_without_errors(false, false, src));
}

void expr_type()
{
    const char* src =
        "#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)\n"
        "static_assert(_is_same(typeof(1 + 2.0), double));";

    assert(compile_without_errors(false, false, src));
}

void expand_test()
{
    char* src =
        "#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)\n"
        "typedef int A[2];"
        "typedef A *B [1];"
        "static_assert(_is_same(typeof(B), int (*[1])[2]));";
    ;

    assert(compile_without_errors(false, false, src));

    //https://godbolt.org/z/WbK9zP7zM
}

void expand_test2()
{

    const char* source
        =
        "\n"
        "\n"
        "typedef char* A;\n"
        "typedef const A* B; \n"
        "static_assert(_Generic(typeof(B), char * const * : 1));\n"
        "\n"
        "typedef const int T;\n"
        "T i;\n"
        "static_assert(_Generic(typeof(i), const int : 1));\n"
        "\n"
        "const T i2;\n"
        "static_assert(_Generic(typeof(i2), const int : 1));\n"
        "\n"
        "typedef  int T3;\n"
        "const T3 i3;\n"
        "static_assert(_Generic(typeof(i3), const int : 1));\n"
        "";


    assert(compile_without_errors(false, false, source));

    //https://godbolt.org/z/WbK9zP7zM
}
void expand_test3()
{


    char* src3 =
        "#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)\n"
        "typedef char* T1;"
        "typedef T1(*f[3])(int); "
        "static_assert(_is_same(typeof(f), char* (* [3])(int)));";

    assert(compile_without_errors(false, false, src3));

    //https://godbolt.org/z/WbK9zP7zM
}

void bigtest()
{
    const char* str =
        "\n"
        "#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)\n"
        "\n"
        "struct X { int i; };\n"
        "\n"
        "struct Y { double d;};\n"
        "\n"
        "enum E { A = 1 };\n"
        "enum E e1;\n"
        "\n"
        "struct X* F() { return 0; }\n"
        "\n"
        "int main()\n"
        "{\n"
        "    enum E { B } e2;\n"
        "    static_assert(_is_same(typeof(e2), enum E));\n"
        "\n"
        "    static_assert(!_is_same(typeof(e2), typeof(e1)));\n"
        "\n"
        "\n"
        "    struct X x;\n"
        "    struct Y y;\n"
        "\n"
        "    static_assert(_is_same(typeof(x), struct X));\n"
        "    static_assert(!_is_same(typeof(x), struct Y));\n"
        "\n"
        "    static_assert(!_is_same(int(double), int()));\n"
        "\n"
        "    int aa[10];\n"
        "\n"
        "    static_assert(_is_same(typeof(*F()), struct X));\n"
        "    static_assert(_is_same(typeof(&aa), int(*)[10]));\n"
        "\n"
        "    int* p = 0;\n"
        "    static_assert(_is_same(typeof(*(p + 1)), int));\n"
        "\n"
        "    static_assert(_is_same(typeof(1), int));\n"
        "\n"
        "    static_assert(_is_same(typeof(main), int()));\n"
        "\n"
        "\n"
        "    static_assert(!_is_same(typeof(main), int(double)));\n"
        "    static_assert(!_is_same(typeof(main), int));\n"
        "\n"
        "\n"
        "    struct X x2;\n"
        "    enum E e;\n"
        "    static_assert(_is_same(typeof(e), enum E));\n"
        "    static_assert(_is_same(typeof(x2), struct X));\n"
        "    static_assert(!_is_same(typeof(e), struct X));\n"
        "\n"
        "\n"
        "\n"
        "    static_assert(_is_same(typeof(1L), long));\n"
        "    static_assert(_is_same(typeof(1UL) , unsigned long));\n"
        "    static_assert(_is_same(typeof(1ULL), unsigned long long));\n"
        "    \n"
        "    //static_assert(_is_same(typeof(A), int));\n"
        "\n"
        "    static_assert(_is_same(typeof(1.0), double));\n"
        "    static_assert(_is_same(typeof(1.0f), float));\n"
        "    static_assert(_is_same(typeof(1.0L), long double));\n"
        "    \n"
        "    \n"
        "    static_assert(_is_same(typeof(((int*)0) + 1), int*));\n"
        "    static_assert(_is_same(typeof(*(((int*)0) + 1)), int));\n"
        "\n"
        "}\n"
        "\n"
        "\n"
        ;
    assert(compile_without_errors(false, false, str));
}

void literal_string_type()
{
    const char* source =
        "#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)\n"
        "    static_assert(_is_same(typeof(\"A\"),  char [2]));\n"
        "    static_assert(_is_same(typeof(\"AB\"),  char [3]));\n"
        ;

    assert(compile_without_errors(false, false, source));
}

void digit_separator_test()
{
    const char* source =
        "static_assert(1'00'00 == 10000);"
        ;

    assert(compile_without_errors(false, false, source));
}

void numbers_test()
{
    const char* source =
        "#if 0xA1 == 161\n"
        "_Static_assert(0xA1 == 161); \n"
        "#endif"
        ;

    assert(compile_without_errors(false, false, source));
}

void binary_digits_test()
{
    const char* source =
        "_Static_assert(0b101010 == 42);"
        "_Static_assert(0b1010'10 == 42);"
        "_Static_assert(052 == 42);"
        ;

    assert(compile_without_errors(false, false, source));
}

void type_suffix_test()
{
    const char* source =
        "\n"
        "#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)\n"
        "\n"
        "static_assert(_is_same(typeof(1), int));\n"
        "static_assert(_is_same(typeof(1L), long));\n"
        "static_assert(_is_same(typeof(1LL), long long));\n"
        "static_assert(_is_same(typeof(1U), unsigned int));\n"
        "static_assert(_is_same(typeof(1ULL), unsigned long long));\n"
        "static_assert(_is_same(typeof(1), int));\n"
        "static_assert(_is_same(typeof(1l), long));\n"
        "static_assert(_is_same(typeof(1ll), long long) );\n"
        "static_assert(_is_same(typeof(1u), unsigned int));\n"
        "static_assert(_is_same(typeof(1ull), unsigned long long));\n"
        "static_assert(_is_same(typeof(0x1), int));\n"
        "static_assert(_is_same(typeof(0x1L), long));\n"
        "static_assert(_is_same(typeof(0x1LL), long long));\n"
        "static_assert(_is_same(typeof(0x1U), unsigned int));\n"
        "static_assert(_is_same(typeof(0x1ULL), unsigned long long));  \n"
        "static_assert(_is_same(typeof(0x1), int));\n"
        "static_assert(_is_same(typeof(0x1l), long));\n"
        "static_assert(_is_same(typeof(0x1ll), long long));\n"
        "static_assert(_is_same(typeof(0x1u), unsigned int));\n"
        "static_assert(_is_same(typeof(0x1ull), unsigned long long));\n"
        "static_assert(_is_same(typeof(0b1), int));\n"
        "static_assert(_is_same(typeof(0b1L), long));\n"
        "static_assert(_is_same(typeof(0b1LL), long long));\n"
        "static_assert(_is_same(typeof(0b1U), unsigned int));\n"
        "static_assert(_is_same(typeof(0b1ULL), unsigned long long));\n"
        "static_assert(_is_same(typeof(0b1l), long));\n"
        "static_assert(_is_same(typeof(0b1ll), long long));\n"
        "static_assert(_is_same(typeof(0b1ul), unsigned long));\n"
        "static_assert(_is_same(typeof(0b1ull), unsigned long long));\n"
        "static_assert(_is_same(typeof(1.0f), float));\n"
        "static_assert(_is_same(typeof(1.0), double));\n"
        "static_assert(_is_same(typeof(1.0L), long double));\n"
        ;


    assert(compile_without_errors(false, false, source));
}

void type_test()
{
    const char* source =
        "#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)\n"
        "int * p = 0;"
        "static_assert(_is_same( typeof( *(p + 1) ), int)   );"
        ;

    assert(compile_without_errors(false, false, source));
}

void is_pointer_test()
{
    const char* source =
        "#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)\n"
        "\n"
        "int main()\n"
        "{\n"
        "  int i;\n"
        "  static_assert(_is_integral(i));\n"
        "  static_assert(_is_floating_point(double) && _is_floating_point(float));\n"
        "  static_assert(_is_function(main));\n"
        "\n"
        "  char * p;\n"
        "  static_assert(_is_scalar(p));\n"
        "  static_assert(_is_scalar(nullptr));\n"
        "\n"
        "  int a[10];\n"
        "  static_assert(_is_array(a));\n"
        "\n"
        "  /*pf = pointer to function (void) returning array 10 of int*/\n"
        "  int (*pf)(void)[10];\n"
        "  static_assert(!_is_array(pf));\n"
        "  static_assert(_is_pointer(pf));\n"
        "\n"
        "  static_assert(_is_same(int, typeof(i)));\n"
        "\n"
        "  static_assert(_is_const(const int));\n"
        "  static_assert(!_is_const(const int*));\n"
        "  static_assert(_is_const(int* const));\n"
        "\n"
        "}\n"
        ;
    assert(compile_without_errors(false, false, source));
}

void params_test()
{
    const char* source =
        "void f1();"
        "void f2(void);"
        "void f3(char * s, ...);"
        "int main()"
        "{"
        "  f1();"
        "  f2();"
        "  f3(\"\");"
        "  f3(\"\", 1, 2, 3);"
        "}"
        ;

    assert(compile_without_errors(false, false, source));
}

void test_compiler_constant_expression()
{
    const char* source =
        "int main()"
        "{"
        "  static_assert('ab' == 'a'*256+'b');\n"
        "  static_assert(sizeof(char)  == 1);\n"
        "  static_assert(true == 1);\n"
        "  static_assert(false == 0);\n"
        "}"
        ;

    assert(compile_without_errors(false, false, source));
}

void zerodiv()
{
    const char* source =
        "int main()"
        "{"
        "  int a = 2/0;\n"
        "}"
        ;

    assert(compile_with_errors(false, false, source));
}

void function_result_test()
{
    const char* source =
        "int (*(*F1)(void))(int, int*);\n"
        "int (* F2(void) )(int, int*);\n"
        "static_assert(_Generic(F1(), int (*)(int, int*) : 1));\n"
        "static_assert(_Generic(F2(), int (*)(int, int*) : 1));\n"
        ;

    assert(compile_without_errors(false, false, source));
}

void type_normalization()
{
    const char* source =
        "char ((a1));\n"
        "char b1;\n"
        "static_assert((typeof(a1)) == (typeof(b1)));\n"
        "\n"
        "char ((a2))[2];\n"
        "char b2[2];\n"
        "static_assert((typeof(a2)) == (typeof(b2)));\n"
        "\n"
        "char ((a3))(int (a));\n"
        "char (b3)(int a);\n"
        "static_assert((typeof(a3)) == (typeof(b3)));\n"
        ;


    assert(compile_without_errors(false, false, source));
}

void auto_test()
{
    const char* source =
        "#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)\n"
        "    int main()\n"
        "    {\n"
        "        double const x = 78.9;\n"
        "        double y = 78.9;\n"
        "        auto q = x;\n"
        "        static_assert( (typeof(q)) == (double));\n"
        "        auto const p = &x;\n"
        "        static_assert( (typeof(p)) == (const double  * const));\n"
        "        auto const r = &y;\n"
        "        static_assert( (typeof(r)) == (double  * const));\n"
        "        auto s = \"test\";\n"
        "        static_assert(_is_same(typeof(s), char *));\n"
        "    }\n"
        ;

    assert(compile_without_errors(false, false, source));

}

void visit_test_auto_typeof()
{
    const char* source = "auto p2 = (typeof(int[2])*) 0;";

    struct report report = {0};
    char* result = compile_source("-std=c99", source, &report);
    assert(strcmp(result, "int  (* p2)[2] = (int(*)[2]) 0;") == 0);
    free(result);
}

void enum_scope()
{
    const char* source =
        "enum E { A = 1 };\n"
        "int main()\n"
        "{\n"
        "  enum E { B } e2; \n"
        "  static_assert( (typeof(e2)), (enum E) ); \n"
        "}\n";
    assert(compile_without_errors(false, false, source));
}

void const_member()
{
    const char* source
        =
        "struct X {\n"
        "  int i;\n"
        "};\n"
        "void f() {\n"
        "  const struct X x = {0};\n"
        "  x.i = 1;\n" //error x.i is constant
        "}\n"
        "";


    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 &&
        report.last_error == C_ASSIGNMENT_OF_READ_ONLY_OBJECT);
}

void register_struct_member()
{
    const char* source
        =
        "struct X {\n"
        "    int i;\n"
        "};\n"
        "\n"
        "int main() {\n"
        "  register struct X x = {0};\n"
        "  int * p = &x.i;\n" //error: address of register variable 'x' requested
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1);
}

void address_of_const()
{
    const char* source =
        "const int i;\n"
        "static_assert(_Generic(&i, const int * : 1 ));\n"
        "\n"
        "const int * const p;\n"
        "static_assert(_Generic(&p, const int *  const * : 1 ));\n"
        "";

    assert(compile_without_errors(false, false, source));
}

void lvalue_test()
{
    //https://en.cppreference.com/w/c/language/value_category

    const char* source
        =
        "//https://en.cppreference.com/w/c/language/value_category\n"
        "\n"
        "struct X\n"
        "{\n"
        "	int i;\n"
        "};\n"
        "\n"
        "struct X f() { struct X x = {0};  return x; }\n"
        "\n"
        "const int i;\n"
        "static_assert(_Generic(&i, const int* : 1));\n"
        "\n"
        "const int* const p;\n"
        "static_assert(_Generic(&p, const int* const* : 1));\n"
        "\n"
        "\n"
        "int main() {\n"
        "	static_assert(!_is_lvalue(1));\n"
        "	static_assert(_is_lvalue(\"a\"));\n"
        "	static_assert(_is_lvalue((int) { 0 }));\n"
        "\n"
        "	int a;\n"
        "	static_assert(_is_lvalue(a));\n"
        "	static_assert(_is_lvalue((a)));\n"
        "\n"
        "	enum  E { A };\n"
        "	static_assert(!_is_lvalue(A));\n"
        "\n"
        "\n"
        "	struct X x = {0};\n"
        "	static_assert(_is_lvalue(x.i));\n"
        "\n"
        "	struct X *px = 0;\n"
        "	static_assert(_is_lvalue(px->i));\n"
        "\n"
        "	//== Non-lvalue object expressions ==\n"
        "	// \n"
        "	//integer, character, and floating constants \n"
        "	static_assert(!_is_lvalue(1));\n"
        "	static_assert(!_is_lvalue(1.2));\n"
        "	static_assert(!_is_lvalue('a'));\n"
        "\n"
        "	//any function call expression\n"
        "	static_assert(!_is_lvalue(f()));\n"
        "	\n"
        "	//any cast expression\n"
        "	static_assert(!_is_lvalue((int)(0)));\n"
        "\n"
        "	//member access operator (dot) applied to a non-lvalue structure/union\n"
        "	static_assert(!_is_lvalue(f().i));\n"
        "	\n"
        "	//results of all arithmetic, relational, logical, and bitwise operators\n"
        "	static_assert(!_is_lvalue(1 + 2));\n"
        "	static_assert(!_is_lvalue(1 && 2));\n"
        "	static_assert(!_is_lvalue(~1));\n"
        "\n"
        "	//results of increment and decrement operators \n"
        "	static_assert(!_is_lvalue(a++));\n"
        "	\n"
        "	//results of assignment operators\n"
        "\n"
        "	//the conditional operator\n"
        "	static_assert(!_is_lvalue(a ? a : a));\n"
        "\n"
        "	//the comma operator\n"
        "	static_assert(!_is_lvalue(a , a)); \n"
        "	\n"
        "	//the address-of operator, even if neutralized by application to the result of unary * operator \n"
        "	static_assert(!_is_lvalue(&a));\n"
        "\n"
        "}\n"
        "";
    ;


    assert(compile_without_errors(false, false, source));
}

void simple_no_discard_test()
{
    const char* source
        =
        "[[nodiscard]] int destroy();\n"
        "\n"
        "int main()\n"
        "{\n"
        "  destroy();\n"
        "}\n"
        "";

    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.warnings_count == 1 && report.last_warning == W_ATTRIBUTES);
}

void simple_no_discard_test2()
{
    const char* source
        =
        "[[nodiscard]] int destroy();\n"
        "\n"
        "int main()\n"
        "{\n"
        "  int i;\n"
        "  i = destroy();\n"
        "}\n"
        "";


    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.warnings_count == 0 && report.error_count == 0);
}

void address_of_register()
{
    const char* source
        =
        "struct X\n"
        "{\n"
        "    int i;\n"
        "};\n"
        "\n"
        "void f()\n"
        "{\n"
        "  register struct X x;\n"
        "  &x;\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_ADDRESS_OF_REGISTER);
}

void return_address_of_local()
{
    const char* source
        =
        "struct X\n"
        "{\n"
        "    int i;\n"
        "};\n"
        "\n"
        "int* f()\n"
        "{\n"
        "  struct X x;\n"
        "  return &x.i;\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.warnings_count == 1 && report.last_warning == W_RETURN_LOCAL_ADDR);
}

void assignment_of_read_only_object()
{
    const char* source
        =
        "struct X\n"
        "{\n"
        "    int i;\n"
        "};\n"
        "\n"
        "int* f()\n"
        "{\n"
        "  const struct X * p;\n"
        "  p->i = 1;\n"
        "}\n";

    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_ASSIGNMENT_OF_READ_ONLY_OBJECT);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////     OWNER /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

void simple_move()
{
    const char* source
        =
        "char * _Owner f() {\n"
        "    char * _Owner p = 0;\n"
        "    return p; /*implicit move*/\n"
        "}";
    assert(compile_without_errors(true, false, source));
}

void simple_move_error()
{
    const char* source
        =
        "char * f() {\n"
        "    char * _Owner p = 0;\n"
        "    return p; \n"
        "}";

    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 &&
        report.last_error == C_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER);
}

void parameter_view()
{
    const char* source
        =
        "\n"
        "struct X { char  * _Owner owner_variable;   };\n"
        "char * f(struct X *parameter) \n"
        "{\n"
        "    return parameter->owner_variable;\n"  //ok to move from parameter
        "}\n";

    assert(compile_without_errors(true, false, source));
}

void move_from_extern()
{
    const char* source
        =
        "struct X { char  * _Owner owner_variable;   };\n"
        "struct X global;\n"
        "char * f() \n"
        "{\n"
        "    return global.owner_variable;\n" /*makes a _View*/
        "}\n";

    assert(compile_without_errors(true, false, source));
}

void owner_type_test()
{
    const char* source
        =
        "\n"
        "struct Y { \n"
        "    char  * _Owner owner_variable;   \n"
        "    char  * non_owner_variable;   \n"
        "};\n"
        "\n"
        "struct X { \n"
        "    char  * _Owner owner_variable;   \n"
        "    char  * non_owner_variable;   \n"
        "    struct Y y1;\n"
        "    _View struct Y y2;\n"
        "};\n"
        "\n"
        "void f()\n"
        "{\n"
        "    struct X x;\n"
        "    \n"
        "    static_assert(_is_owner(typeof(x)));\n"
        "    static_assert(_is_owner(typeof(x.owner_variable)));\n"
        "    static_assert(!_is_owner(typeof(x.non_owner_variable)));\n"
        "    static_assert(_is_owner(struct X));\n"
        "    static_assert(_is_owner(typeof(x.y1)));\n"
        "    static_assert(!_is_owner(typeof(x.y2)));\n"
        "    \n"
        "    static_assert(_is_owner(typeof(x.y1.owner_variable)));\n"
        "    static_assert(!_is_owner(typeof(x.y1.non_owner_variable)));\n"
        "\n"
        "    static_assert(!_is_owner(typeof(x.y2.owner_variable)));\n"
        "    static_assert(!_is_owner(typeof(x.y2.non_owner_variable)));\n"
        "\n"
        "    _View struct X x2;\n"
        "    static_assert(!_is_owner(typeof(x2)));\n"
        "    static_assert(!_is_owner(typeof(x2.owner_variable)));\n"
        "    static_assert(!_is_owner(typeof(x2.non_owner_variable)));\n"
        "\n"
        "    _Owner char * p;\n"
        "    static_assert(!_is_owner(typeof(p)));\n"
        "    static_assert(_is_owner(typeof(*p)));    \n"
        "}\n";

    assert(compile_without_errors(true, false, source));
}

void correct_move_assigment()
{
    const char* source
        =
        "\n"
        "struct Y { \n"
        "    int i;\n"
        "};\n"
        "\n"
        "struct X { \n"
        "    char * _Owner name;\n"
        "};\n"
        "\n"
        "int main()\n"
        "{\n"
        "    struct Y y1 = {};\n"
        "    struct Y y2 = {};\n"
        "    y1 = y2; //ok\n"
        "\n"
        "    struct X x1 = {};\n"
        "    struct X x2 = {};\n"
        "    x1 = x2; //ok\n"
        "\n"
        "}";
    assert(compile_without_errors(true, false, source));
}

void no_explicit_move_required()
{
    const char* source
        =
        "char * _Owner create();\n"
        "void f(char * _Owner p);\n"
        "\n"
        "int main()\n"
        "{\n"
        "    f(create());\n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));

}

void no_explicit_move_with_function_result()
{
    const char* source
        =
        "void destroy(char* _Owner x);\n"
        "char   * _Owner  get();\n"
        "\n"
        "int main()\n"
        "{\n"
        "  destroy(get());\n"
        "}\n";

    assert(compile_without_errors(true, false, source));
}

void cannot_ignore_owner_result()
{
    const char* source
        =
        "struct X {\n"
        "  char * _Owner name;\n"
        "};\n"
        "\n"
        "struct X f();\n"
        "\n"
        "int main()\n"
        "{\n"
        "  f();\n"
        "}\n";

    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.warnings_count == 1);
}

void can_ignore_owner_result()
{
    const char* source
        =
        "struct X {\n"
        "  char * _Owner name;\n"
        "};\n"
        "\n"
        "_View struct X f();\n"
        "\n"
        "int main()\n"
        "{\n"
        "  f();\n"
        "}\n";

    assert(compile_without_errors(true, false, source));
}

void move_not_necessary_on_return()
{
    const char* source
        =
        "struct X {\n"
        "  char * _Owner name;\n"
        "};\n"
        "\n"
        "struct X f();\n"
        "struct X f2()\n"
        "{\n"
        "    return f();\n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void explicit_move_not_required()
{
    const char* source
        =
        "#define NULL ((void*)0)\n"
        "\n"
        "int main()\n"
        "{\n"
        "    const char * _Owner s;\n"
        "    s = NULL;    \n"
        "    s = 0;    \n"
        "    s = nullptr;    \n"
        "}\n"
        ;
    assert(compile_without_errors(true, false, source));
}

void error_using_temporary_owner()
{
    const char* source
        =
        "\n"
        "void F(int i);\n"
        "_Owner int make();\n"
        "int main()\n"
        "{\n"
        "    F(make());\n"
        "}";
    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_OWNERSHIP_USING_TEMPORARY_OWNER);

}

void passing_view_to_owner()
{
    const char* source
        =
        "void destroy(_Owner int i);\n"
        "\n"
        "int main()\n"
        "{\n"
        "  _Owner int i = 0;\n"
        "  int v = i;\n"
        "  destroy(v);\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER);
}

void obj_owner_cannot_be_used_in_non_pointer()
{
    const char* source
        =
        "void f() {\n"
        "    _Obj_owner int i;\n"
        "}\n"
        ;
    struct options options = {.input = LANGUAGE_C99, .enabled_warnings_stack[0] = (~0 & ~W_STYLE)};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_OBJ_OWNER_CAN_BE_USED_ONLY_IN_POINTER);

}

void ownership_flow_test_null_ptr_at_end_of_scope()
{
    const char* source
        =
        "void f() {\n"
        "    _Owner int * p = 0;\n"
        "}\n"
        " ";
    assert(compile_without_errors(true, false, source));
}

void ownership_flow_test_pointer_must_be_deleted()
{
    const char* source
        =
        "\n"
        "int* _Owner  get();\n"
        "\n"
        "void f() {\n"
        "    int * _Owner p = 0;\n"
        "    p = get();\n"
        "}\n"
        " ";
    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_OWNERSHIP_FLOW_MISSING_DTOR);
}

void ownership_flow_test_basic_pointer_check()
{
    const char* source
        =
        "\n"
        "int* _Owner  get();\n"
        "void dtor(int* _Owner p);\n"
        "\n"
        "void f(int a)\n"
        "{\n"
        "    int* _Owner p = 0;\n"
        "    p = get();    \n"
        "    dtor(p);    \n"
        "}\n"
        "";

    assert(compile_without_errors(true, false, source));
}

void ownership_flow_test_struct_member_missing_free()
{
    const char* source
        =
        "\n"
        "char * _Owner strdup(const char* s);\n"
        "void free(void* _Owner p);\n"
        "\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "};\n"
        "\n"
        "void f(int a)\n"
        "{\n"
        "    struct X x = {0};\n"
        "    x.text = strdup(\"a\");\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_OWNERSHIP_FLOW_MISSING_DTOR);
    ////TODO return ROOT object!

}

void ownership_flow_test_struct_member_free()
{
    const char* source
        =
        "\n"
        "char * _Owner strdup(const char* s);\n"
        "void free(void* _Owner p);\n"
        "\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "};\n"
        "\n"
        "void f(int a)\n"
        "{\n"
        "    struct X x = {0};\n"
        "    x.text = strdup(\"a\");\n"
        "    free(x.text);\n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));

}

void ownership_flow_test_move_inside_if()
{
    const char* source
        =
        "void free( void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "\n"
        "void f(int c) \n"
        "{\n"
        "    int * _Owner p = malloc(sizeof (int));    \n"
        "    if (c) {\n"
        "      free(p);\n"
        "    }\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1);
}

void ownership_flow_test_goto_same_scope()
{
    const char* source
        =
        "void free( void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "\n"
        "void f(int condition) \n"
        "{\n"
        "    int * _Owner p = malloc(sizeof(int));\n"
        "  \n"
        "    if (condition)\n"
        "       goto end;\n"
        "  end:\n"
        "    free(p);\n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void ownership_flow_test_jump_labels()
{
    const char* source
        =
        "void free( void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "\n"
        "void f(int condition)\n"
        "{\n"
        "    int* _Owner p = malloc(sizeof(int));\n"
        "\n"
        "    if (condition)\n"
        "        goto end;\n"
        "\n"
        "    free(p);\n"
        "end:\n"
        "\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C2X, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_OWNERSHIP_FLOW_MISSING_DTOR);
}

void ownership_flow_test_owner_if_pattern_1()
{
    const char* source
        =
        "\n"
        "void free( void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "\n"
        "int main()\n"
        "{\n"
        "    int* _Owner p = malloc(sizeof(int));\n"
        "    if (p)\n"
        "    {\n"
        "       free(p);     \n"
        "    }\n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void ownership_flow_test_owner_if_pattern_2()
{
    const char* source
        =
        "\n"
        "void free( void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "\n"
        "int main()\n"
        "{\n"
        "    int* _Owner p = malloc(sizeof(int));\n"
        "    if (p != 0)\n"
        "    {\n"
        "       free(p);     \n"
        "    }\n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void ownership_flow_test_missing_destructor()
{
    const char* source
        =
        "struct X {\n"
        "  _Owner i;\n"
        "};\n"
        "void f() {\n"
        "  const struct X x = {0};\n"
        "}\n"
        "";


    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.last_error == C_OWNERSHIP_FLOW_MISSING_DTOR);

}

void ownership_flow_test_no_warning()
{
    const char* source
        =
        "void free( void * _Owner p);\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "};\n"
        "void x_delete( struct X * _Owner p)\n"
        "{\n"
        "    if (p)\n"
        "    {\n"
        "      free(p->text);\n"
        "      free(p);\n"
        "    }\n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void ownership_flow_test_moved_if_not_null()
{
    const char* source
        =
        "void * _Owner malloc(int i);\n"
        "void free( void * _Owner p);\n"
        "\n"
        "struct X { int i; };\n"
        "struct Y { struct X * _Owner p; };\n"
        "\n"
        "int main() {\n"
        "   struct Y y = {0};\n"
        "   struct X * _Owner p = malloc(sizeof(struct X));\n"
        "   if (p){\n"
        "     y.p = p;\n"
        "   }\n"
        "  free(y.p);\n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void ownership_flow_test_struct_moved()
{
    const char* source
        =
        "void free( void * _Owner p);\n"
        "\n"
        "struct X {\n"
        "  char * _Owner name;\n"
        "};\n"
        "\n"
        "void x_destroy( struct X * _Obj_owner p);\n"
        "\n"
        "struct Y {\n"
        "  struct X x;\n"
        "};\n"
        "\n"
        "void y_destroy(struct Y * _Obj_owner p) {\n"
        "   x_destroy(&p->x);\n"
        "}\n"
        ;
    assert(compile_without_errors(true, false, source));
}

void ownership_flow_test_scope_error()
{
    const char* source
        =
        "void * _Owner malloc(int i);\n"
        "void free( void* _Owner p);\n"
        "\n"
        "int main() {\n"
        "    try\n"
        "    {\n"
        "         if (1)\n"
        "         {\n"
        "             char * _Owner s = malloc(1);\n"
        "             free(s);\n"
        "         }\n"
        "         else\n"
        "         {\n"
        "            throw;\n"
        "         }\n"
        "    }\n"
        "    catch\n"
        "    {\n"
        "    }\n"
        "}";
    assert(compile_without_errors(true, false, source));
}


void ownership_flow_test_void_destroy()
{
    /*TODO moving to void* requires object is moved before*/
    const char* source
        =
        "void * _Owner malloc(int i);\n"
        "void free( void * _Owner p);\n"
        "\n"
        "struct X {\n"
        "  char * _Owner name;    \n"
        "};\n"
        "\n"
        "int main() {\n"
        "   struct X * _Owner p = malloc(sizeof * p);\n"
        "   free(p);   \n"
        "} \n"
        ;

    assert(compile_without_errors(true, false, source));
}

void ownership_flow_test_void_destroy_ok()
{
    /*TODO moving to void* requires object is moved before*/
    const char* source
        =
        "void * _Owner malloc(int i);\n"
        "void free( void * _Owner p);\n"
        "\n"
        "struct X {\n"
        "  char * _Owner name;    \n"
        "};\n"
        "\n"
        "int main() {\n"
        "   struct X * _Owner p = malloc(sizeof * p);\n"
        "   p->name = malloc(10);\n"
        "   free(p->name);\n"
        "   free(p);   \n"
        "} \n"
        ;
    assert(compile_without_errors(true, false, source));
}

void ownership_flow_test_moving_owner_pointer()
{
    const char* source
        =
        "\n"
        "void * _Owner malloc(int i);\n"
        "void free( void * _Owner p);\n"
        "\n"
        "struct X {\n"
        "  char * _Owner name;    \n"
        "};\n"
        "\n"
        "void x_delete( struct X * _Owner p)\n"
        "{\n"
        "  if (p) {\n"
        "      free(p->name);\n"
        "      free(p);\n"
        "  }\n"
        "}\n"
        "\n"
        "int main() {\n"
        "   struct X * _Owner p = malloc(sizeof * p);   \n"
        "   x_delete(p);      \n"
        "} \n"
        "";
    assert(compile_with_errors(true, false, source));
}

void ownership_flow_test_moving_owner_pointer_missing()
{
    const char* source
        =
        "\n"
        "void * _Owner malloc(int i);\n"
        "void free( void * _Owner p);\n"
        "\n"
        "struct X {\n"
        "  char * _Owner name;    \n"
        "};\n"
        "\n"
        "void x_delete( struct X * _Owner p)\n"
        "{\n"
        "  if (p) {\n"
        "      //free(p->name);\n"
        "      free(p);\n"
        "  }\n"
        "}\n"
        "\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.warnings_count == 0);
}

void ownership_flow_test_error()
{
    const char* source
        =
        "\n"
        "void* _Owner malloc(int size);\n"
        "\n"
        "struct X {    \n"
        "    char * _Owner name;\n"
        "};\n"
        "\n"
        "void * _Owner f1(){\n"
        "  struct X * _Owner p = malloc(sizeof (struct X));\n"
        "  p->name = malloc(1);  \n"
        "  return p;\n"
        "}\n"
        "";

    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.warnings_count == 0);
}

void ownership_flow_test_setting_owner_pointer_to_null()
{
    const char* source
        =
        "\n"
        "void * _Owner malloc(int i);\n"
        "void free( void * _Owner p);\n"
        "\n"
        "struct X {\n"
        "  char * _Owner name;    \n"
        "};\n"
        "\n"
        "int main() {\n"
        "   struct X * _Owner p = malloc(sizeof * p);   \n"
        "   p = 0;\n"
        "} \n"
        "";
    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.warnings_count == 0);
}

void ownership_flow_test_while_not_null()
{
    const char* source
        =
        "struct item  {\n"
        "    struct item * _Owner next;\n"
        "};\n"
        "void item_delete( struct item * _Owner p);\n"
        "\n"
        "struct list {\n"
        "    struct item * _Owner head;\n"
        "    struct item * tail;\n"
        "};\n"
        "int main()\n"
        "{\n"
        "    struct list list = {0};\n"
        "    struct item * _Owner p = list.head;\n"
        "    while (p){\n"
        "      struct item * _Owner next = p->next;\n"
        "      p->next = 0;\n"
        "      item_delete(p);\n"
        "      p = next;\n"
        "  }  \n"
        "}";
    assert(compile_without_errors(true, false, source));
}

void ownership_flow_test_if_state()
{
    const char* source
        =
        "\n"
        "int* _Owner make();\n"
        "void free(int * _Owner p);\n"
        "\n"
        "\n"
        "void f(int condition)\n"
        "{\n"
        "  int * _Owner p = 0;\n"
        "  static_state(p, \"null\");\n"
        "  \n"
        "  if (condition)\n"
        "  {\n"
        "       static_state(p, \"null\");   \n"
        "       p = make();\n"
        "       static_state(p, \"maybe-null\");\n"
        "  }\n"
        "  else\n"
        "  {\n"
        "    static_state(p, \"null\");\n"
        "  }\n"
        "  free(p);\n"
        "}\n"
        "\n"
        "";

    assert(compile_without_errors(true, false, source));
}

void ownership_types_test_error_owner()
{
    const char* source
        =
        "void * f();\n"
        "int main() {\n"
        "   void * _Owner p = f();   \n"
        "}\n"
        ;
    struct options options = {.input = LANGUAGE_C99};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.warnings_count == 0);
}

void ownership_flow_test_if_variant()
{
    const char* source
        =
        "void * _Owner f();\n"
        "void free( void *_Owner p);\n"
        "int main() {\n"
        "   void * _Owner p = f();   \n"
        "   if (p)\n"
        "   {\n"
        "       free(p);\n"
        "       p = f();   \n"
        "   }\n"
        "}\n"
        "";


    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.warnings_count == 0);
}

void check_leaks_on_else_block()
{
    const char* source
        =
        "void * _Owner malloc(int sz);\n"
        "\n"
        "void f(int i) {   \n"
        "        if (i){\n"
        "        }   \n"
        "        else {\n"
        "            int * _Owner p3 = malloc(1);\n"
        "        }\n"
        "}\n"
        ;

    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1 && report.warnings_count == 0);
}

void ownership_flow_test_two_ifs()
{
    const char* source
        =
        "void * _Owner malloc(int sz);\n"
        "void free( void * _Owner opt p);\n"
        "\n"
        "\n"
        "void f(int i) {   \n"
        "    void * _Owner p = 0;\n"
        "    if (i)\n"
        "    {\n"
        "        if (i)\n"
        "        {\n"
        "            p =  malloc(1);\n"
        "        }\n"
        "        else\n"
        "        {\n"
        "            p = malloc(1);\n"
        "        }     \n"
        "    }\n"
        "    \n"
        "    free(p);\n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));

}

void ownership_no_name_parameter()
{
    const char* source
        =
        "void free( void * _Owner){ }\n"
        "";

    assert(compile_with_errors(true, false, source));
}

void ownership_flow_switch_case()
{
    const char* source
        =
        "void* _Owner make();\n"
        "void free( void* _Owner p);\n"
        "\n"
        "void f(condition)\n"
        "{\n"
        "    void* _Owner p = make();\n"
        "\n"
        "\n"
        "    switch (condition)\n"
        "    {\n"
        "        case 1:\n"
        "        {\n"
        "            free(p);\n"
        "        }\n"
        "        break;\n"
        "        case 2:\n"
        "        {\n"
        "            free(p);\n"
        "        }\n"
        "        break;\n"
        "\n"
        "        default:\n"
        "            free(p);\n"
        "            break;\n"
        "    }        \n"
        "}";
    assert(compile_without_errors(true, false, source));
}

void state_inner_objects_preserved()
{
    const char* source
        =
        "void *_Owner malloc(int i);\n"
        "void free(void  *_Owner);\n"
        "\n"
        "struct X{\n"
        "  char * _Owner name;\n"
        "};\n"
        "\n"
        "int main()\n"
        "{\n"
        "    struct X * _Owner p = malloc(sizeof(struct X));    \n"
        "    if (p)\n"
        "    {\n"
        "        p->name = malloc(1);\n"
        "    }\n"
        "    else \n"
        "    {        \n"
        "        p->name = malloc(1);\n"
        "    }\n"
        "    free(p->name);\n"
        "    free(p);\n"
        "}";
    assert(compile_without_errors(true, false, source));
}

//TODO make test with
// f(void (*pf)(void* _Owner p)){}
// 
void owner_parameter_must_be_ignored()
{
    const char* source = "void f(void (*pf)(void* _Owner p)){}";
    assert(compile_without_errors(true, false, source));
}

void taking_address()
{
    const char* source
        =
        "struct X {\n"
        "  void * _Owner text;\n"
        "};\n"
        "\n"
        "void x_change(struct X* list);\n"
        "void x_destroy(struct X* _Obj_owner p);\n"
        "\n"
        "int main()\n"
        "{\n"
        "  struct X x = {};\n"
        "  static_debug(x);\n"
        "  x_change(&x);\n"
        "  //list_destroy(&list);\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 1);
}

void taking_address_const()
{
    const char* source
        =
        "struct X {\n"
        "  void * _Owner text;\n"
        "};\n"
        "\n"
        "void f(const struct X* list);\n"
        "\n"
        "int main()\n"
        "{\n"
        "  struct X x = {};\n"
        "  f(&x);\n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void pointer_argument()
{
    const char* source
        =
        "void * _Owner malloc(int i);\n"
        "\n"
        "struct X {\n"
        "  void * _Owner text;\n"
        "};\n"
        "\n"
        "void x_change( struct X* list);\n"
        "\n"
        "int main()\n"
        "{\n"
        "  struct X * _Owner x = malloc(sizeof * x);\n"
        "  x_change(x);\n"
        "}\n"
        "";
    struct options options = {.input = LANGUAGE_C99, .flow_analysis = true};
    struct report report = {0};
    get_ast(&options, "source", source, &report);
    assert(report.error_count == 3);
}

void do_while()
{
    const char* source
        =
        "\n"
        "void* _Owner malloc(unsigned size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "int main() {\n"
        "   void * _Owner p = malloc(1);\n"
        "   do{\n"
        "      free(p);\n"
        "   }\n"
        "   while(0);   \n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void switch_cases_state()
{
    const char* source
        =
        "void* _Owner malloc(unsigned size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "void* _Owner f(int i)\n"
        "{\n"
        "    void* _Owner p = malloc(1);\n"
        "    switch (i)\n"
        "    {\n"
        "        case 1:\n"
        "            break;\n"
        "        case 2:\n"
        "            break;\n"
        "    }\n"
        "\n"
        "    return p;\n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void switch_break()
{
    const char* source
        =
        "void * _Owner malloc(int i);\n"
        "\n"
        "void* _Owner f(int i)\n"
        "{\n"
        "    void* _Owner p = malloc(1);        \n"
        "    switch (i) {\n"
        "        case 1: break;\n"
        "    }\n"
        "    return p;\n"
        "}";
    assert(compile_without_errors(true, false, source));
}

void passing_non_owner()
{
    /*
      We need to analuse better this case...

    */
    const char* source
        =
        "struct X { \n"
        "    char * _Owner p;\n"
        "};\n"
        "void x_destroy(struct X * _Obj_owner p);\n"
        "void f(struct X* x)\n"
        "{\n"
        "    x_destroy(x);        \n"
        "}\n"
        "";

    assert(compile_with_errors(true, false, source));
}

void flow_analysis_else()
{
    const char* source

        =
        "void * _Owner malloc(int i);\n"
        "void free(void * _Owner p);\n"
        "\n"
        "int main() {\n"
        "    int * _Owner p1 = 0;\n"
        "    int * _Owner p2 = malloc(1);\n"
        "\n"
        "    if (p2 == 0) {\n"
        "        return 1;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "      p1 = p2;\n"
        "    }\n"
        "    static_state(p2, \"moved\");\n"
        "    free(p1);\n"
        "    return 0;\n"
        "}";

    "}";

    assert(compile_without_errors(true, false, source));
}
void moving_content_of_owner()
{
    const char* source
        =
        "\n"
        "void* _Owner malloc(unsigned size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "\n"
        "struct X {  char *_Owner name; };\n"
        "struct Y { struct X x; };\n"
        "\n"
        "void f(struct Y * y, struct X * _Obj_owner p) \n"
        "{\n"
        "    free(y->x.name);\n"
        "    y->x = *p;\n"
        "}\n"
        ;
    assert(compile_without_errors(true, false, source));
}

void switch_scope()
{
    const char* source
        =
        "\n"
        "void* _Owner calloc(unsigned n, unsigned size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "struct X {\n"
        "    char* _Owner name;\n"
        "};\n"
        "\n"
        "struct X* _Owner F(int i)\n"
        "{\n"
        "    struct X* _Owner p1 = 0;\n"
        "\n"
        "    switch (i)\n"
        "    {\n"
        "        case 1:\n"
        "            struct X* _Owner p2 = calloc(1, sizeof * p2);\n"
        "            if (p2)\n"
        "            {\n"
        "              static_set(*p2, \"zero\");\n"
        "                p1 = p2;\n"
        "            }\n"
        "            break;\n"
        "        case 2:\n"
        "            break;\n"
        "    }\n"
        "\n"
        "    return p1;\n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void swith_and_while()
{
    const char* source
        =
        "\n"
        "void* _Owner malloc(unsigned size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "struct X {\n"
        "    char* _Owner name;\n"
        "};\n"
        "\n"
        "struct X* _Owner F(int i)\n"
        "{\n"
        "\n"
        "    struct X* _Owner p1 = 0;\n"
        "    try\n"
        "    {\n"
        "        if (i == 1)\n"
        "        {\n"
        "            p1 = malloc(sizeof * p1);            \n"
        "            while (0){}            \n"
        "        }\n"
        "        else if (i == 3)\n"
        "        {\n"
        "            p1 = malloc(sizeof * p1);\n"
        "        }\n"
        "    }\n"
        "    catch\n"
        "    {\n"
        "    }\n"
        "\n"
        "    return p1;\n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void owner_to_non_owner()
{
    const char* source
        =
        "void * f();\n"
        "int main() {\n"
        "  void * _Owner p = f();\n"
        "}";

    assert(compile_with_errors(true, false, source));
}

void owner_to_non_owner_zero()
{
    const char* source
        =
        "void * f();\n"
        "int main() {\n"
        "  void * _Owner p = 0;\n"
        "}";

    assert(compile_without_errors(true, false, source));
}

void incomplete_struct()
{
    const char* source
        =
        "void free(void * _Owner p);\n"
        "struct X;\n"
        "struct X f();\n"
        "struct X { char * _Owner p; };\n"
        "int main()\n"
        "{\n"
        "    struct X x = 1 ? f() : f(); \n"
        "    free(x.p);\n"
        "}";
    assert(compile_without_errors(true, false, source));

}

void switch_pop_problem()
{
    const char* source
        =
        "\n"
        "void* _Owner malloc(unsigned size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "\n"
        "void f(int i)\n"
        "{\n"
        "  void * _Owner p1 = malloc(1);\n"
        "  switch(i)\n"
        "  {\n"
        "      case 1:\n"
        "      {\n"
        "          void * _Owner p2 = malloc(1);\n"
        "          free(p2);\n"
        "      }\n"
        "      break;\n"
        "\n"
        "      case 2:\n"
        "      {\n"
        "          void * _Owner p3 = malloc(1);\n"
        "            free(p3);\n"
        "      }\n"
        "      break;\n"
        "  }\n"
        "\n"
        "  free(p1);\n"
        "  \n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void switch_pop2()
{
    const char* source
        =
        "\n"
        "void* _Owner malloc(unsigned size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "\n"
        "void f(int i)\n"
        "{\n"
        "    void* _Owner p1 = malloc(1);\n"
        "    switch (i)\n"
        "    {\n"
        "        case 1:\n"
        "            void* _Owner p2 = malloc(1);\n"
        "            free(p2);\n"
        "            break;\n"
        "\n"
        "        case 3:\n"
        "            void* _Owner p3 = malloc(1);\n"
        "            free(p3);\n"
        "            break;\n"
        "    }\n"
        "\n"
        "    free(p1);\n"
        "\n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void scopes_pop()
{
    const char* source
        =
        "\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "struct X { char* _Owner name; };\n"
        "\n"
        "void x_destroy(struct X* _Obj_owner p);\n"
        "struct X f();\n"
        "\n"
        "void f()\n"
        "{\n"
        "    {\n"
        "        struct X x = {0};   \n"
        "        \n"
        "        if (1)\n"
        "        {            \n"
        "            x = f();\n"
        "        }\n"
        "        else\n"
        "        {         \n"
        "            x = f();\n"
        "        }\n"
        "        x_destroy(&x);\n"
        "    }\n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}
void owner_moved()
{
    const char* source
        =
        "void free( void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "struct X { char * _Owner text; };\n"
        "\n"
        "void x_destroy(struct X* _Obj_owner p)\n"
        "{\n"
        "    free(p->text);\n"
        "}\n"
        "\n"
        "void x_delete(struct X* _Owner p)\n"
        "{\n"
        "    if (p)\n"
        "    {\n"
        "        x_destroy(p);\n"
        "        free(p);\n"
        "    }\n"
        "}";
    assert(compile_without_errors(true, false, source));

}

void partially_owner_moved()
{
    const char* source
        =
        "void free( void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "struct X { char * _Owner text; };\n"
        "\n"
        "void x_destroy(struct X* _Obj_owner p)\n"
        "{\n"
        "    free(p->text);\n"
        "}\n"
        "\n"
        "void x_delete(struct X* _Owner p)\n"
        "{\n"
        "    if (p)\n"
        "    {\n"
        "        x_destroy(p);\n"
        "    }\n"
        "}";
    assert(compile_with_errors(true, false, source));

}
void use_after_destroy()
{
    const char* source
        =
        "\n"
        "char* _Owner strdup(const char* s);\n"
        "void* _Owner malloc(unsigned size);\n"
        "void free(void* _Owner ptr);"
        "\n"
        "struct X {\n"
        "  char *_Owner name;\n"
        "};\n"
        "\n"
        "void x_destroy(struct X * _Obj_owner p) {\n"
        "  free(p->name);\n"
        "}\n"
        "\n"
        "void x_print(struct X * p) \n"
        "{\n"
        "  //printf(\"%s\", p->name);\n"
        "}\n"
        "\n"
        "int main() {\n"
        "   struct X x = {0};\n"
        "   x.name = strdup(\"a\");\n"
        "   x_destroy(&x);\n"
        "   x_print(&x);\n"
        "}\n"
        "";
    assert(compile_with_errors(true, false, source));

}

void obj_owner_must_be_from_addressof()
{
    const char* source
        =
        "void free(void* _Owner ptr);\n"
        "void* _Owner malloc(int size);\n"
        "char * _Owner strdup(const char* );\n"
        "\n"
        "struct X {\n"
        "  char *_Owner name;\n"
        "};\n"
        "\n"
        "struct Y {\n"
        "  struct X x;\n"
        "  struct X * px;\n"
        "};\n"
        "\n"
        "void x_destroy(struct X * _Obj_owner p) \n"
        "{\n"
        "  free(p->name);\n"
        "}\n"
        "\n"
        "void f(struct Y * p)\n"
        "{    \n"
        "    x_destroy(p->px);\n"
        "}\n"
        "\n"
        "int main() {\n"
        "   struct Y  y = {};   \n"
        "   struct * p = &y.x;\n"
        "   x_destroy(&y.x);\n"
        "}\n"
        "\n"
        "";
    assert(compile_with_errors(true, false, source));
}

void discarding_owner()
{
    const char* source
        =
        "void* _Owner malloc(unsigned long size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "struct X {\n"
        "  char *_Owner name;\n"
        "};\n"
        "\n"
        "int main()\n"
        "{  \n"
        "  struct X * p = (struct X * _Owner) malloc(1);\n"
        "}";
    assert(compile_with_errors(true, false, source));
}

void using_uninitialized()
{
    const char* source
        =
        "void* _Owner malloc(unsigned long size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "};\n"
        "\n"
        "void x_delete(struct X * _Owner  p);\n"
        "\n"
        "int main() {   \n"
        "   struct X * _Owner p = malloc(sizeof(struct X));      \n"
        "   x_delete(p); /*uninitialized*/\n"
        "}\n"
        "\n"
        "";
    assert(compile_with_errors(true, false, source));
}

void using_uninitialized_struct()
{
    const char* source
        =
        "struct X {\n"
        "  char * _Owner text;\n"
        "};\n"
        "\n"
        "void x_destroy(struct X * _Obj_owner p);\n"
        "\n"
        "\n"
        "int main() {   \n"
        "   struct X x;\n"
        "   x_destroy(&x);\n"
        "}\n"
        "\n"
        "";

    assert(compile_with_errors(true, false, source));
}

void zero_initialized()
{
    const char* source
        =
        "struct Y {\n"
        "  char * _Owner p0;\n"
        "  int * _Owner p2;\n"
        "  double i2;\n"
        "};\n"
        "\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "  int * _Owner p1;\n"
        "  int i;\n"
        "  struct Y  *pY;\n"
        "};\n"
        "\n"
        "int main() {   \n"
        "   struct X x = {0};   \n"
        "   static_state(x.text, \"null\");\n"
        "   static_state(x.p1, \"null\");\n"
        "   static_state(x.i, \"zero\");\n"
        "   static_state(x.pY, \"null\");\n"
        "   static_state(x.pY->p0, \"\");\n"
        "   static_state(x.pY->p2, \"\");\n"
        "   static_state(x.pY->i2, \"\");\n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));
}


void empty_initialized()
{
    const char* source
        =
        "struct Y {\n"
        "  char * _Owner p0;\n"
        "  int * _Owner p2;\n"
        "  double i2;\n"
        "};\n"
        "\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "  int * _Owner p1;\n"
        "  int i;\n"
        "  struct Y  *pY;\n"
        "};\n"
        "\n"
        "int main() {   \n"
        "   struct X x = {};   \n"
        "   static_state(x.text, \"null\");\n"
        "   static_state(x.p1, \"null\");\n"
        "   static_state(x.i, \"zero\");\n"
        "   static_state(x.pY, \"null\");\n"
        "   static_state(x.pY->p0, \"\");\n"
        "   static_state(x.pY->p2, \"\");\n"
        "   static_state(x.pY->i2, \"\");\n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void calloc_state()
{
    const char* source
        =
        "\n"
        "void* _Owner calloc(unsigned long n , unsigned long size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "struct Y {\n"
        "  char * _Owner p0;\n"
        "  int * _Owner p2;\n"
        "  double i2;\n"
        "};\n"
        "\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "  int * _Owner p1;\n"
        "  int i;\n"
        "  struct Y  *pY;\n"
        "};\n"
        "\n"
        "int main() {   \n"
        "   struct X * _Owner x = calloc(1,sizeof * x);\n"
        "   static_state(x, \"maybe-null\");\n"
        "\n"
        "   static_state(x->p1, \"null\");\n"
        "   static_state(x->i, \"zero\");\n"
        "   static_state(x->pY, \"null\");\n"
        "   static_state(x->pY->p0, \"uninitialized\");\n"
        "   static_state(x->pY->p2, \"uninitialized\");\n"
        "   static_state(x->pY->i2, \"uninitialized\");   \n"
        "   free(x);\n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void malloc_initialization()
{
    const char* source
        =
        "\n"
        "void* _Owner malloc(unsigned long size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "struct Y {\n"
        "  char * _Owner p0;\n"
        "  int * _Owner p2;\n"
        "  double i2;\n"
        "};\n"
        "\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "  int * _Owner p1;\n"
        "  int i;\n"
        "  struct Y  *pY;\n"
        "};\n"
        "\n"
        "int main() {   \n"
        "   struct X * _Owner x = malloc(sizeof * x);\n"
        "   static_state(x, \"maybe-null\");\n"
        "\n"
        "   static_state(x->p1, \"uninitialized\");\n"
        "   static_state(x->i, \"uninitialized\");\n"
        "   static_state(x->pY, \"uninitialized\");\n"
        "   static_state(x->pY->p0, \"\");\n"
        "   static_state(x->pY->p2, \"\");\n"
        "   static_state(x->pY->i2, \"\");   \n"
        "   free(x);\n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void valid_but_unkown_result()
{
    const char* source
        =
        "\n"
        "void* _Owner malloc(unsigned long size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "struct Y {\n"
        "  char * _Owner p0;\n"
        "  int * _Owner p2;\n"
        "  double i2;\n"
        "};\n"
        "\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "  int * _Owner p1;\n"
        "  int i;\n"
        "  struct Y  *pY;\n"
        "};\n"
        "\n"
        "struct X f();\n"
        "\n"
        "int main() {   \n"
        "   struct X x;\n"
        "   x = f();\n"
        "\n"
        "   static_state(x.p1, \"maybe-null\");\n"
        "   static_state(x.i, \"any\");\n"
        "   static_state(x.pY, \"maybe-null\");\n"
        "   static_state(x.pY->p0, \"maybe-null\");\n"
        "   static_state(x.pY->p2, \"maybe-null\");\n"
        "   static_state(x.pY->i2, \"any\");   \n"
        "   free(x);\n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void calling_non_const_func()
{
    const char* source
        =
        "\n"
        "void* _Owner malloc(unsigned long size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "struct Y {\n"
        "  char * _Owner p0;\n"
        "  int * _Owner p2;\n"
        "  double i2;\n"
        "};\n"
        "\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "  int * _Owner p1;\n"
        "  int i;\n"
        "  struct Y  *pY;\n"
        "};\n"
        "\n"
        "void init(struct X * p);\n"
        "\n"
        "int main() {   \n"
        "   struct X x;\n"
        "   /*lying here, to avoid error of using uninitialized*/\n"
        "   static_set(x,\"zero\");\n"
        "   init(&x);\n"
        "\n"
        "   static_state(x.p1, \"maybe-null\");\n"
        "   static_state(x.i, \"any\");\n"
        "   static_state(x.pY, \"maybe-null\");\n"
        "   static_state(x.pY->p0, \"maybe-null\");\n"
        "   static_state(x.pY->p2, \"maybe-null\");\n"
        "   static_state(x.pY->i2, \"any\");   \n"
        "   free(x);\n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));
}
void calling_const_func()
{
    const char* source
        =
        "void* _Owner malloc(unsigned long size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "struct Y {\n"
        "    char* _Owner p0;\n"
        "    int* _Owner p2;\n"
        "    double i2;\n"
        "};\n"
        "\n"
        "struct X {\n"
        "    char* _Owner text;\n"
        "    int* _Owner p1;\n"
        "    int i;\n"
        "    struct Y* pY;\n"
        "};\n"
        "\n"
        "void f(const struct X* p);\n"
        "\n"
        "int main()\n"
        "{\n"
        "    struct X x = {0};\n"
        "    f(&x);\n"
        "\n"
        "    static_state(x.p1, \"null\");\n"
        "    static_state(x.i, \"zero\");\n"
        "    static_state(x.pY, \"null\");\n"
        "    static_state(x.pY->p0, \"\");\n"
        "    static_state(x.pY->p2, \"\");\n"
        "    static_state(x.pY->i2, \"\");\n"
        "\n"
        "    free(x);\n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));
}
void pointer_to_owner()
{
    const char* source
        =
        "\n"
        "void* _Owner malloc(unsigned long size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "struct X {\n"
        "  char * _Owner text;\n"
        "};\n"
        "\n"
        "void f(struct X * _Owner p1, struct X * _Owner* p2){\n"
        "  *p2 = p1;\n"
        "}\n"
        "\n"
        "int main() {   \n"
        "   struct X * _Owner p1 = malloc(sizeof * p1);\n"
        "   p1->text = 0;\n"
        "   struct X * _Owner p2 = 0;\n"
        "   f(p1, &p2);\n"
        "   \n"
        "   free(p2->text);\n"
        "   free(p2);\n"
        "}\n"
        "\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void socket_sample()
{
    const char* source
        =
        "_Owner int socket();\n"
        "void close(_Owner int fd);\n"
        "\n"
        "int main()\n"
        "{\n"
        "  _Owner int fd;\n"
        "  \n"
        "  fd = socket();\n"
        "  if (fd < 0)\n"
        "  {\n"
        "     static_set(fd, \"null\");   \n"
        "     return 1;\n"
        "  }\n"
        "  close(fd);\n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void return_object()
{
    const char* source
        =
        "char * _Owner strdup(const char* s);\n"
        "void free(void * _Owner p);\n"
        "\n"
        "struct X {\n"
        "  char *_Owner name;\n"
        "};\n"
        "\n"
        "struct X make()\n"
        "{\n"
        "  struct X x = {0};\n"
        "  x.name = strdup(\"text\");  \n"
        "  return x;\n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}
void return_bad_object()
{
    const char* source
        =
        "char * _Owner strdup(const char* s);\n"
        "void free(void * _Owner p);\n"
        "\n"
        "struct X {\n"
        "  char *_Owner name;\n"
        "};\n"
        "\n"
        "struct X make()\n"
        "{\n"
        "  struct X x = {0};\n"
        "  x.name = strdup(\"text\");  \n"
        "  free(x.name)\n"
        "  return x;\n"
        "}\n"
        "";
    assert(compile_with_errors(true, false, source));
}

void null_to_owner()
{
    const char* source
        =
        "\n"
        "void f(int * _Owner p);\n"
        "int main()\n"
        "{\n"
        "   int * _Owner p = 0;\n"
        "   p = ((void *) 0); \n"
        "   f(0);\n"
        "   f((void *) 0);\n"
        "   f(nullptr);\n"
        "}\n";
    assert(compile_without_errors(true, false, source));
}

void return_true_branch()
{

    const char* source
        =
        "void* _Owner malloc(unsigned long size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "void f5()\n"
        "{\n"
        "    void * _Owner p = malloc(1);\n"
        "    \n"
        "    if (p) {\n"
        "       free(p);\n"
        "       return;\n"
        "    }\n"
        "    \n"
        "    static_state(p, \"null\");    \n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}
void flow_tests()
{
    const char* source
        =
        "\n"
        "\n"
        "void* _Owner malloc(unsigned long size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "void f1()\n"
        "{\n"
        "    void * _Owner p = malloc(1);\n"
        "    if (p) {\n"
        "      static_state(p, \"not-null\");\n"
        "    }\n"
        "\n"
        "    static_state(p, \"maybe-null\");\n"
        "    free(p);\n"
        "}\n"
        "\n"
        "void f2(int condition)\n"
        "{\n"
        "    void * _Owner p = malloc(1);\n"
        "    if (condition) {\n"
        "      static_state(p, \"maybe-null\");\n"
        "    }\n"
        "\n"
        "    static_state(p, \"maybe-null\");\n"
        "    static_set(p, \"null\");\n"
        "}\n"
        "\n"
        "void f3(int condition)\n"
        "{\n"
        "    void * _Owner p = malloc(1);\n"
        "    \n"
        "    if (condition) {\n"
        "       free(p);\n"
        "    }\n"
        "    else {\n"
        "       free(p);\n"
        "    }\n"
        "\n"
        "    static_state(p, \"uninitialized\");    \n"
        "}\n"
        "\n"
        "void f3(int condition)\n"
        "{\n"
        "    void * _Owner p = malloc(1);\n"
        "    \n"
        "    if (condition) {\n"
        "       \n"
        "    }\n"
        "    else {\n"
        "       free(p);\n"
        "    }\n"
        "\n"
        "    static_state(p, \"uninitialized or maybe_null\");    \n"
        "    static_set(p, \"null\");\n"
        "}\n"
        "\n"
        "\n"
        "void f4(int condition)\n"
        "{\n"
        "    void * _Owner p = malloc(1);\n"
        "    \n"
        "    if (condition) {\n"
        "       free(p);\n"
        "    }\n"
        "    else {\n"
        "       \n"
        "    }\n"
        "\n"
        "    static_state(p, \"uninitialized or maybe_null\");   \n"
        "    static_set(p, \"null\");\n"
        "}\n"
        "\n"
        "void f5(int condition)\n"
        "{\n"
        "    void * _Owner p = malloc(1);\n"
        "    \n"
        "    if (p) {\n"
        "       free(p);\n"
        "       return;\n"
        "    }\n"
        "    \n"
        "    static_state(p, \"null\");    \n"
        "}\n"
        "\n"
        "";
    assert(compile_without_errors(true, false, source));
}

void member()
{
    const char* source
        =
        "struct X {\n"
        "  union {\n"
        "    struct {\n"
        "      int *pSelect;\n"
        "    } view;\n"
        "  }u;\n"
        "};\n"
        "\n"
        "int main()\n"
        "{\n"
        "    struct X t;\n"
        "    t.u.view.pSelect = 0;\n"
        "}\n"
        "";
    assert(compile_without_errors(true, false, source));
}
void loop_leak()
{
    const char* source
        =
        "void* _Owner malloc(unsigned long size);\n"
        "void free(void* _Owner ptr);\n"
        "\n"
        "int main() {\n"
        "   void * _Owner p = 0;\n"
        "   for (int i=0; i < 2; i++) {\n"
        "     p = malloc(1);\n"
        "   }\n"
        "   free(p);\n"
        "}";
    assert(compile_with_errors(true, false, source));
}

void out_parameter()
{
    const char* source
        =
        "void  free(void* _Owner p);\n"
        "char* _Owner strdup(const char* s);\n"
        "\n"
        "struct X {\n"
        "    char* _Owner s;\n"
        "};\n"
        "void init(_Out struct X *  px)\n"
        "{\n"
        "    static_state(px, \"maybe-null\");\n"
        "    static_state(px->s, \"uninitialized\");\n"
        "    px->s = strdup(\"a\");\n"
        "}\n"
        "\n"
        "int main() {\n"
        "    struct X x;\n"
        "    init(&x);\n"
        "    free(x.s);\n"
        "}";

    assert(compile_without_errors(true, false, source));
}

void lvalue_required_1()
{
    const char* source
        =
        "int main()\n"
        "{\n"
        " 1++;\n"
        "}\n"
        "";
    assert(compile_with_errors(true, false, source));
}

void lvalue_required_2()
{
    const char* source
        =
        "int main()\n"
        "{\n"
        " 1--;\n"
        "}\n"
        "";
    assert(compile_with_errors(true, false, source));
}

void lvalue_required_3()
{
    const char* source
        =
        "int main()\n"
        "{\n"
        " int * p = &1;\n"
        "}\n"
        "";
    assert(compile_with_errors(true, false, source));
}

void null_check_1()
{
    const char* source
        =
        "void f(int  *p)\n"
        "{\n"
        " static_state(p, \"not-null\");\n"
        "}\n"
        "";
    assert(compile_without_errors(true, true, source));
}

void null_check_2()
{
    const char* source
        =
        "void f(int  *p)\n"
        "{\n"
        " static_state(p, \"maybe-null\");\n"
        "}\n"
        "";
    assert(compile_without_errors(true, false /*nullcheck disabled*/, source));
}


#endif

