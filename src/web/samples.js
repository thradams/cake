var sample = {};

sample["c11"] = [];

sample["c11"]["alignas-alignas-in-c23"] =
`
#include <stdio.h>\n\n// every object of type struct sse_t will be aligned to 16-byte boundary\n// (note: needs support for DR 444)\nstruct sse_t\n{\n    alignas(16) float sse_data[4];\n};\n\n// every object of type struct data will be aligned to 128-byte boundary\nstruct data\n{\n    char x;\n    alignas(128) char cacheline[128]; // over-aligned array of char,\n                                      // not array of over-aligned chars\n};\n\nint main(void)\n{\n    printf(\"sizeof(data) = %zu (1 byte + 127 bytes padding + 128-byte array)\\n\",\n           sizeof(struct data));\n\n    printf(\"alignment of sse_t is %zu\\n\", alignof(struct sse_t));\n\n    alignas(2048) struct data d; // this instance of data is aligned even stricter\n    (void)d; // suppresses \"maybe unused\" warning\n}\n\n/*\n   OUTPUT\n   sizeof(data) = 256 (1 byte + 127 bytes padding + 128-byte array)\n   alignment of sse_t is 16\n*/\n
`;

sample["c11"]["alignof-alignof-in-c23"] =
`
struct X\n{\n    char s;\n    //align member 7\n    double c;\n    int s2;\n    //align member 4\n};\n\nint main(void)\n{\n    static_assert(_Alignof(float[10]) == alignof(float));\n    static_assert(alignof(char) == 1);\n    static_assert(sizeof(struct X) == 24);\n    static_assert(alignof(struct X) == 8);\n}\n
`;

sample["c11"]["anonymous-structures-unions"] =
`
struct point { float x, y, z; };\n\nstruct location {\n    char* name;\n    struct point;\n};\n\nint main()\n{\n    struct location location = {};\n    location.x = 1;\n    location.y = 2;\n}\n
`;

sample["c11"]["generic"] =
`
#include <math.h>\n\n#define cbrt(X) _Generic((X), \\\n                  double: cbrtl, \\\n                  float: cbrtf ,\\\n                  default: cbrtl  \\\n              )(X)\n\nint main(void)\n{\n    cbrt(1.0);\n\n    const int * const p = 0;\n    _Static_assert(_Generic(p, const int * : 1 ), \"\");\n    _Static_assert(_Generic(&p, const int * const * : 1 ), \"\");\n    _Static_assert(_Generic(main, int (*)(void) : 1 ), \"\");\n\n    const int * const p2 = 0;\n    static_assert(_Generic(p2, const int *: 1));\n\n    static_assert(_Generic(\"abc\",  char *: 1));    \n}\n
`;

sample["c11"]["noreturn"] =
`
#include <stdio.h>\n#include <stdlib.h>\n\n_Noreturn void not_coming_back(void)\n{\n    puts(\"There's no coming back\");\n    exit(1);\n    return;\n}\n
`;

sample["c11"]["static-assert"] =
`
/*\n   See also C23 static_assert\n*/\n\nint main()\n{\n    _Static_assert(1 == 1, \"error\");    \n}\n
`;

sample["c11"]["thread-local-thread-local-in-c23"] =
`
thread_local int a;\n\nint main(void)\n{    \n}\n
`;

sample["c11"]["u-u-char-literals"] =
`
int main()\n{\n  int i = U'ç';\n  int i2 = u'ç';\n}\n
`;

sample["c11"]["u8-literals"] =
`
/*\n* cake input source code encode is always utf8\n* cake output source code is also utf8\n\n* u8 prefix may be useful in case you have a compiler where\n* the input or output is not uft8.\n*/\n\n#include <stdio.h>\n\nint main()\n{\n  printf(\"Hello, 世界\\n\");\n  printf(u8\"Hello, 世界\\n\");\n}\n
`;

sample["c11"]["x86_msvc\\alignof-alignof-in-c23"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:54:16 */\n\nint main(void)\n{\n}\n
`;

sample["c11"]["x86_msvc\\anonymous-structures-unions"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:54:19 */\n\nstruct point {\n    float x;\n    float y;\n    float z;\n};\n\nstruct location {\n    char * name;\n    struct point __m0;\n};\n\nstatic void * memset(void *dest, int ch, unsigned int count);\n\nint main()\n{\n    struct location location;\n\n    memset(&location, 0, 16);\n    location.__m0.x = 1;\n    location.__m0.y = 2;\n}\n\nstatic void * memset(void *ptr, int value, unsigned int count)\n{\n    unsigned char *p;\n    unsigned char v;\n\n    p = (unsigned char *) ptr;\n    v = (unsigned char) value;\n    while (count--)\n    {\n        *p++ = v;\n    }\n\n    return ptr;\n}\n
`;

sample["c11"]["x86_msvc\\generic"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:54:20 */\n\nlong double __cdecl cbrtl(long double _X);\n\nint main(void)\n{\n    int * p;\n    int * p2;\n\n    cbrtl(1.0);\n    p = 0;\n    p2 = 0;\n}\n
`;

sample["c11"]["x86_msvc\\noreturn"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:54:22 */\n\nint __cdecl puts(char * _Buffer);\nvoid __cdecl exit(int _Code);\n\nvoid not_coming_back(void)\n{\n    puts(\"There's no coming back\");\n    exit(1);\n    return;\n}\n
`;

sample["c11"]["x86_msvc\\static-assert"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:54:23 */\n\nint main()\n{\n}\n
`;

sample["c11"]["x86_msvc\\thread-local-thread-local-in-c23"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:54:25 */\n__declspec(thread) int a;\n\nint main(void)\n{\n}\n
`;

sample["c11"]["x86_msvc\\u-u-char-literals"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:54:26 */\n\nint main()\n{\n    int i;\n    int i2;\n\n    i = 231;\n    i2 = 231;\n}\n
`;

sample["c11"]["x86_msvc\\u8-literals"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:54:27 */\n\nstruct _iobuf {\n    void * _Placeholder;\n};\n\nstruct __crt_multibyte_data;\n\nstruct __crt_locale_data;\n\nstruct __crt_locale_pointers {\n    struct __crt_locale_data * locinfo;\n    struct __crt_multibyte_data * mbcinfo;\n};\n\nstatic int __cdecl printf(char * _Format, ...);\nstatic int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);\nint __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);\nstatic unsigned long long *__cdecl __local_stdio_printf_options(void);\nstatic unsigned long long _OptionsStorage;\nstruct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);\n\nint main()\n{\n    printf(\"Hello, \\xe4\\xb8\\x96\\xe7\\x95\\x8c\\n\");\n    printf(\"Hello, \\xe4\\xb8\\x96\\xe7\\x95\\x8c\\n\");\n}\n\nstatic unsigned long long *__cdecl __local_stdio_printf_options(void)\n{\n    return &_OptionsStorage;\n}\n\nstatic int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)\n{\n    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);\n}\n\nstatic int __cdecl printf(char * _Format, ...)\n{\n    int _Result;\n    char * _ArgList;\n\n    ((void)(_ArgList = (char *)(&(_Format)) + 4));\n    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);\n    ((void)(_ArgList = (char *)0));\n    return _Result;\n}\n
`;

sample["c23"] = [];

sample["c23"]["auto"] =
`
\n\n/*function sample*/\nextern int func(void);\nauto p_func = func;\nconst auto pc_func = func;\n\n/*using auto inside a macro*/\n#define SWAP(a, b)   do {    auto temp = a; a = b; b = temp;   } while (0)\n\nvoid f()\n{\n  int a = 1;\n  int b = 2;\n  SWAP(a, b);\n}\n\nauto literal_string = \"hello auto\";\n\nstruct {int i;} x;\n\n\nauto bb = true;\nauto pnull = nullptr;\n\n/*arrays*/\nint a5[5];\nauto a = a5; /*lvalue*/\n\nauto pa = &a5;\n\nconst auto cpa = &a5;\n\nvoid f2(int a[2]){\n  auto p = a;\n}\n\n\nint main()\n{\n  double const x = 78.9;\n  double y = 78.9;\n  auto q = x;\n  auto const p = &x;\n  auto const r = &y;\n}\n
`;

sample["c23"]["binary-literal"] =
`
int main()\n{\n    int b = 0B10;\n}\n
`;

sample["c23"]["bitint-n"] =
`
//TODO\n
`;

sample["c23"]["bool-true-false"] =
`
\n#include <stdio.h>\n\nint main()\n{\n  bool b = true;\n  b = false;\n  static_assert(1 == true);\n  static_assert(0 == false);\n\n  printf(\"%s\", _Generic(true, bool : \"bool\"));\n  printf(\"%s\", _Generic(false, bool : \"bool\"));\n\n  printf(\"%s\", _Generic(b, bool : \"bool\"));\n\n  auto b2 = true;\n  printf(\"%s\", _Generic(b2, bool : \"bool\"));\n}\n
`;

sample["c23"]["constexpr"] =
`
#include <stdio.h>\n\nconstexpr int c = 123;\n\nconstexpr int c2 = c + 1000;\n\nint a[c];\n\nconstexpr double PI = 3.14;\n\n\nstatic_assert(PI + 1 == 3.14 + 1.0);\n\nstruct Y {\n    int a;\n    int ar[3];\n    int b;\n};\n\nvoid T3()\n{\n    constexpr struct Y y = { .ar[1] = 2, 3, 4 };\n    static_assert(y.a == 0);\n    static_assert(y.ar[0] == 0);\n    static_assert(y.ar[1] == 2);\n    static_assert(y.ar[2] == 3);\n    static_assert(y.b == 4);\n    static_assert(y.ar[1] + y.ar[2] == 5);\n}\n\nstatic_assert(\"abc\"[0] == 'a');\n\n\nint main()\n{\n    constexpr char ch = 'a';\n\n    printf(\"%f %c\", PI, ch);\n}\n
`;

sample["c23"]["deprecated"] =
`
[[deprecated]] void f2() {\n}\n\n\nstruct [[deprecated]] S {\n  int a;\n};\n\nenum [[deprecated]] E1 {\n one\n};\n\nint main(void) {\n    struct S s;\n    enum E1 e;\n    f2();\n}\n
`;

sample["c23"]["digit-separator"] =
`
int main()\n{\n    int a = 1000'00;\n    static_assert(1000'00 == 100000);\n}\n
`;

sample["c23"]["elifdef-elifndef"] =
`
/*\n  C23 preprocessing directives elifdef and elifndef N2645\n  https://open-std.org/jtc1/sc22/wg14/www/docs/n2645.pdf\n*/\n\n#define Y\n\n#ifdef X\n#define VERSION 1\n#elifdef  Y\n#define VERSION 2\n#else\n#define VERSION 3\n#endif\n\n_Static_assert(VERSION == 2, \"\");\n
`;

sample["c23"]["embed"] =
`
#include <stdio.h>\n\nint main()\n{\n  static const char file_txt[] = {\n   #embed \"stdio.h\"\n   ,0\n  };\n\n  printf(\"%s\\n\", file_txt);\n\n}\n\n/* \n  Note\n  The idea is to have a mode where cake generates a file to \n  be included like this:\n\n  static const char file_txt[] = {\n   #include \"stdio.h.embed\"\n   ,0\n  };\n\n*/\n
`;

sample["c23"]["empty-initializer"] =
`
int main()\n{\n    struct X {\n        int i;\n    } x = {};\n\n    x = (struct X) {};\n\n    struct Y\n    {\n        struct X x;\n    } y = { {} };\n}\n
`;

sample["c23"]["enhancements-to-enumerations"] =
`
//https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3030.htm\nenum X : short {\n    A\n};\n\nint main() {\n    enum X x = A;\n    static_assert(sizeof(x) == sizeof(short));\n}\n
`;

sample["c23"]["fallthrough"] =
`
/*\n   IS NOT IMPLEMENTED YET\n*/\n\nvoid f(int n) {\n    void g(void), h(void), i(void);\n    switch (n) {\n    case 1: /* diagnostic on fallthrough discouraged */\n    case 2:\n        g();\n        [[fallthrough]];\n    case 3: /* diagnostic on fallthrough discouraged */\n        do {\n            [[fallthrough]]; /* constraint violation: next statement is not\n            part of the same secondary block execution */\n        } while (false);\n    case 6:\n        do {\n            [[fallthrough]]; /* constraint violation: next statement is not\n            part of the same secondary block execution */\n        } while (n--);\n    case 7:\n        while (false) {\n            [[fallthrough]]; /* constraint violation: next statement is not\n            part of the same secondary block execution */\n        }\n    case 5:\n        h();\n    case 4: /* fallthrough diagnostic encouraged */\n        i();\n        [[fallthrough]]; /* constraint violation */\n    }\n}\n
`;

sample["c23"]["has-include-has-embed-has-c-attribute"] =
`
\n#if __has_include(<stdio.h>)\n#warning  yes we have <stdio.h>\n#endif\n\n\n#if __has_embed(<stdio.h>)\n#warning  yes we have <stdio.h> embed\n#endif\n\n\n#if __has_include(<otherfile.h>)\n#warning  YES\n#else\n#warning  NO we dont have <otherfile.h>\n#endif\n\n\n#if __has_c_attribute(fallthrough)\n#else\n#warning at this moment we return 0 for all attributes\n#endif\n\n/*\n  __has_include is a sample of feature that is impossible to translate, \n  unless for immediate compilation.\n*/\n
`;

sample["c23"]["improved-enumerations"] =
`
//TODO\n//https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3029.htm\n\nenum a {\n\ta0 = 0xFFFFFFFFFFFFFFFFULL\n};\n\nint main () {}\n
`;

sample["c23"]["labels"] =
`
/* https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2508.pdf */\nint main()\n{\n  goto label;\n\n  label:\n  int x;\n\n  goto exit;\n  exit:\n}\n
`;

sample["c23"]["maybe-unused"] =
`
\nvoid f( [[maybe_unused]] int arg1, int arg2)\n{\n    [[maybe_unused]] int local1;\n    int local2;\n    /*warning not used for local2*/\n    /*warning not used for arg2*/\n}\n
`;

sample["c23"]["nodiscard"] =
`
\n#include <stdlib.h>\n\nstruct [[nodiscard]] error_info { int error; };\n\nstruct error_info enable_missile_safety_mode(void);\n\nvoid launch_missiles(void);\n\nvoid test_missiles(void) {\n    enable_missile_safety_mode();\n    launch_missiles();\n}\n\n[[nodiscard(\"must check armed state\")]]\nbool arm_detonator(int within);\n\nvoid detonate();\n\nvoid call(void) {\n  arm_detonator(3);\n  detonate();\n}\n
`;

sample["c23"]["nullptr"] =
`
\n#include <stdlib.h>\n#include <stdio.h>\n\nint main()\n{\n  void * p = nullptr;\n  void * p2 = NULL;\n\n  auto a = nullptr;\n  \n  printf(\"%s\", _Generic(nullptr, typeof(nullptr) : \"nullptr_t\"));\n}\n
`;

sample["c23"]["static-assert"] =
`
//https://en.cppreference.com/w/c/language/_Static_assert\n#include <assert.h> // no longer needed since C23\n\nint main(void)\n{\n    // Test if math works, C23:\n    static_assert((2 + 2) % 3 == 1, \"Whoa dude, you knew!\");\n    // Pre-C23 alternative:\n    _Static_assert(2 + 2 * 2 == 6, \"Lucky guess!?\");\n\n    // This will produce an error at compile time.\n    // static_assert(sizeof(int) < sizeof(char), \"Unmet condition!\");\n\n    constexpr int _42 = 2 * 3 * 2 * 3 + 2 * 3;\n    static_assert(_42 == 42); // the message string can be omitted.\n\n    // const int _13 = 13;\n    // Compile time error - not an integer constant expression:\n    // static_assert(_13 == 13);\n}\n
`;

sample["c23"]["static-compound-literal"] =
`
// https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3038.htm\nint main()\n{\n    int* p = (static int[]) {1, 2, 3};\n}\n
`;

sample["c23"]["typeof-typeof-unqual"] =
`
#include <stdlib.h>\n\n#define SWAP(a, b) \\\n  do {\\\n    typeof(a) temp = a; a = b; b = temp; \\\n  } while (0)\n\n\n/*pointer to function returning int*/\nint (*g1)(int a);\ntypeof(g1) g2;\n\n/*function returning int* */\nint * f1(int a);\n\n/*function returning int* */\ntypeof(f1) f2;\n\n/*pointer to function returning int*/\ntypeof(f1) *f3;\n\n\ntypeof(1 + 1) f()\n{\n  return 1;\n}\n\nvoid f4(int a[2]) {\n    typeof(a) p;\n}\n\nint main()\n{\n    /*simple case*/\n    int a = 1;\n    typeof(a) b = 1;\n\n    /*pay attention to the pointer*/\n    typeof(int*) p1, p2;\n    typeof(typeof(int*)) p3;\n    typeof(typeof(p1)) p4;\n    \n\n    typeof_unqual(const int) p5;\n    typeof_unqual(const int * const) p6;\n    \n    \n\n    /*let's expand this macro and see inside*/\n    SWAP(a, b);\n\n    /*for anonymous structs we insert a tag*/\n    struct { int i; } x;\n    typeof(x) x2;\n    typeof(x) x3;\n\n   /*Things get a little more complicated*/\n   int *array[2];\n   typeof(array) a1, a2;\n   \n   typeof(array) a3[3];\n   typeof(array) *a4[4];\n\n   /*abstract declarator*/\n   int k = sizeof(typeof(array));\n\n\n   /*new way to declare pointer to functions?*/\n   typeof(void (int)) * pf = NULL;\n}\n\n\nint f5(){\n  typeof(int [2]) *p1 = 0;\n  auto p2 = (typeof(int [2]) *) p1 ;\n}\n
`;

sample["c23"]["u8-char-literal"] =
`
//https://open-std.org/JTC1/SC22/WG14/www/docs/n2418.pdf\nint main()\n{\n   unsigned char c = u8'~';\n}\n
`;

sample["c23"]["va-opt"] =
`
/*\n  __VA_OPT__ lets you optionally insert tokens depending on\n  if a variadic macro is invoked with additional arguments. \n  \n  (Select: Compile To Preprocess only)\n*/\n\n#define M(X, ...) X __VA_OPT__(,) __VA_ARGS__\nM(1)    // expands to 1\nM(1, 2) // expands to 1, 2\n\n#define F(...) f(0 __VA_OPT__(,) __VA_ARGS__)\n#define G(X, ...) f(0, X __VA_OPT__(,) __VA_ARGS__)\n#define SDEF(sname, ...) S sname __VA_OPT__(= { __VA_ARGS__ })\n#define EMP\nF(a, b, c)           // replaced by f(0, a, b, c)\nF()                  // replaced by f(0)\nF(EMP)               // replaced by f(0)\nG(a, b, c)           // replaced by f(0, a, b, c)\nG(a, )               // replaced by f(0, a)\nG(a)                 // replaced by f(0, a)\nSDEF(foo);           // replaced by S foo;\nSDEF(bar, 1, 2);     // replaced by S bar = { 1, 2 };\n\n\n#define H2(X, Y, ...) __VA_OPT__(X ## Y,) __VA_ARGS__\nH2(a, b, c, d)       // replaced by ab, c, d\n\n#define H3(X, ...) #__VA_OPT__(X##X X##X)\nH3(, 0)              // replaced by \"\"\n  \n#define H4(X, ...) __VA_OPT__(a X ## X) ## b\nH4(, 1)              // replaced by a b\n\n#define H5A(...) __VA_OPT__()/**/__VA_OPT__()\n#define H5B(X) a ## X ## b\n#define H5C(X) H5B(X)\nH5C(H5A())          // replaced by ab\n
`;

sample["c23"]["warning"] =
`
#include <stdio.h>\n\nint main()\n{\n  #warning TODO ..missing code  \n}\n
`;

sample["c23"]["x86_msvc\\auto"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:54:35 */\n\nstruct __tag0 {\n    int i;\n};\n\nextern int func(void);\nint (* p_func)(void) = func;\nint (* pc_func)(void) = func;\n\nvoid f()\n{\n    int a;\n    int b;\n\n    a = 1;\n    b = 2;\n    do\n    {\n        int temp;\n\n        temp = a;\n        a = b;\n        b = temp;\n    }\n    while (0);\n}\nchar * literal_string = \"hello auto\";\nstruct __tag0 x;\nunsigned char bb = 1;\nnullptr_t pnull = 0;\nint a5[5];\nint * a = a5;\nint (* pa)[5] = &a5;\nint (* cpa)[5] = &a5;\n\nvoid f2(int a[2])\n{\n    int * p;\n\n    p = a;\n}\n\nint main()\n{\n    double x;\n    double y;\n    double q;\n    double * p;\n    double * r;\n\n    x = 78.900000000000006;\n    y = 78.900000000000006;\n    q = 78.900000000000006;\n    p = &x;\n    r = &y;\n}\n
`;

sample["c23"]["x86_msvc\\bitint-n"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:54:37 */\n
`;

sample["c23"]["x86_msvc\\bool-true-false"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:54:38 */\n\nstruct _iobuf {\n    void * _Placeholder;\n};\n\nstruct __crt_multibyte_data;\n\nstruct __crt_locale_data;\n\nstruct __crt_locale_pointers {\n    struct __crt_locale_data * locinfo;\n    struct __crt_multibyte_data * mbcinfo;\n};\n\nstatic int __cdecl printf(char * _Format, ...);\nstatic int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);\nint __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);\nstatic unsigned long long *__cdecl __local_stdio_printf_options(void);\nstatic unsigned long long _OptionsStorage;\nstruct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);\n\nint main()\n{\n    unsigned char b;\n    unsigned char b2;\n\n    b = 1;\n    b = 0;\n    printf(\"%s\", \"bool\");\n    printf(\"%s\", \"bool\");\n    printf(\"%s\", \"bool\");\n    b2 = 1;\n    printf(\"%s\", \"bool\");\n}\n\nstatic unsigned long long *__cdecl __local_stdio_printf_options(void)\n{\n    return &_OptionsStorage;\n}\n\nstatic int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)\n{\n    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);\n}\n\nstatic int __cdecl printf(char * _Format, ...)\n{\n    int _Result;\n    char * _ArgList;\n\n    ((void)(_ArgList = (char *)(&(_Format)) + 4));\n    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);\n    ((void)(_ArgList = (char *)0));\n    return _Result;\n}\n
`;

sample["c23"]["x86_msvc\\constexpr"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:56:17 */\n\nstruct Y {\n    int a;\n    int ar[3];\n    int b;\n};\n\nstruct _iobuf {\n    void * _Placeholder;\n};\n\nstruct __crt_multibyte_data;\n\nstruct __crt_locale_data;\n\nstruct __crt_locale_pointers {\n    struct __crt_locale_data * locinfo;\n    struct __crt_multibyte_data * mbcinfo;\n};\nint c = 123;\nint c2 = 1123;\nint a[123];\ndouble PI = 3.1400000000000001;\n\nvoid T3()\n{\n    struct Y y;\n\n    y.a = 0;\n    y.ar[0] = 0;\n    y.ar[1] = 2;\n    y.ar[2] = 3;\n    y.b = 4;\n}\n\nstatic int __cdecl printf(char * _Format, ...);\nstatic int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);\nint __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);\nstatic unsigned long long *__cdecl __local_stdio_printf_options(void);\nstatic unsigned long long _OptionsStorage;\nstruct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);\n\nint main()\n{\n    char ch;\n\n    ch = 97;\n    printf(\"%f %c\", 3.1400000000000001, 97);\n}\n\nstatic unsigned long long *__cdecl __local_stdio_printf_options(void)\n{\n    return &_OptionsStorage;\n}\n\nstatic int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)\n{\n    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);\n}\n\nstatic int __cdecl printf(char * _Format, ...)\n{\n    int _Result;\n    char * _ArgList;\n\n    ((void)(_ArgList = (char *)(&(_Format)) + 4));\n    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);\n    ((void)(_ArgList = (char *)0));\n    return _Result;\n}\n
`;

sample["c2y"] = [];

sample["c2y"]["case-range"] =
`
  //Case range expressions\n  //https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3370.htm\n\n  #include <stdio.h>\n\n  void f(int n)\n  {\n    switch (n)\n    {\n       case 1 ... 10:\n       printf(\"n in range 1...10\\n\");\n       break;\n       default:\n       break;\n    }\n  }\n\n  int main(){\n    f(1);\n    f(11);\n  }\n
`;

sample["c2y"]["counter"] =
`
//https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3457.htm#number-of-expansions\n\n#define X(Z) Z Z\nX(__COUNTER__) // 0 0\nX(__COUNTER__) // 1 1\n
`;

sample["c2y"]["countof"] =
`
/*\n  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3369.pdf\n*/\n\nvoid f(int n)\n{\n  int v[123][n];\n  static_assert(_Countof(v) == 123);\n}\n\nint main()\n{\n  int a[7][3];\n  int n = _Countof(a);\n  static_assert(_Countof(a) == 7);\n\n  int n2 = _Countof(int [7][3]);\n  static_assert(_Countof(int [2][3]) == 2);\n}\n
`;

sample["c2y"]["def-ii"] =
`
  // Add directives #def and #enddef\n  // https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3524.txt\n\n/*\n  use -preprocess-def-macro\n  to preprocess #def macro after expansion\n*/\n\n\n#def foo(x)\n\t\tdo {\n#if 0\n\t\t\tbar(x);\n#endif\n\t\t\tbaz(x);\n\t\t}\n        while (0)\n#enddef\n\nfoo(2)\n
`;

sample["c2y"]["def"] =
`
/*\n  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3524.txt\n*/\n\n#def foo(x)\n\t\tdo {\n\t\t\tbar(x);\n\t\t\tbaz(x);\n\t\t}\n        while (0)\n#enddef\n\nfoo(1)\nfoo(2)\n
`;

sample["c2y"]["defer-and-flow-analysis"] =
`
#pragma flow enable\n\nvoid f2(int i){}\nvoid f(int k)\n{\n  int i;\n  _Defer f2(i);\n  \n  if (k > 1)\n   return;\n}\n
`;

sample["c2y"]["defer-goto-vi"] =
`
\n#include <stdio.h>\n\nint main()\n{\n  FILE* f = fopen(\"in.txt\", \"r\");\n  if (f != NULL)\n  {\n     _Defer fclose(f);\n\n     FILE* f2 = fopen(\"out.txt\", \"w\");\n     if (f2 == NULL) goto LEND;\n     _Defer fclose(f2);\n  }\n  LEND:\n  return 0;\n}\n
`;

sample["c2y"]["defer-inside-try-blocks"] =
`
/*\n  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3199.htm\n*/\n\n#include <stdio.h>\n\nint main()\n{\n\n  try\n  {\n     FILE* f = fopen(\"in.txt\", \"r\");\n     if (f == NULL) throw;\n     _Defer fclose(f);\n\n     FILE* f2 = fopen(\"out.txt\", \"w\");\n     if (f2 == NULL) throw;\n     _Defer fclose(f2);\n\n     //...\n\n    /*success here*/\n  }\n  catch\n  {\n     /*some error*/\n  }\n\n\n}\n
`;

sample["c2y"]["defer-interleaved-with-return"] =
`
\nint f(){\n  int i = 1;\n  _Defer {\n    i = 0;\n  }\n  return i++;\n}\n\nvoid f0(){\n  int i = 1;\n  _Defer {\n    i = 0;\n  }\n  return; //empty or constant expression\n}\n
`;

sample["c2y"]["defer-with-breaks-iii"] =
`
\n#include <stdio.h>\n\nint main()\n{\n\n  do\n  {\n     FILE* f = fopen(\"in.txt\", \"r\");\n     if (f == NULL) break;\n     _Defer fclose(f);\n\n     FILE* f2 = fopen(\"out.txt\", \"w\");\n     if (f2 == NULL) break;\n     _Defer fclose(f2);\n\n     //...\n\n    /*success here*/\n  }\n  while(0);\n\n\n}\n
`;

sample["c2y"]["defer-with-breaks-iv"] =
`
\n#include <stdio.h>\n\nint main()\n{\n  FILE* f = NULL;\n  _Defer if (f) fclose(f);\n\n  do\n  {\n     f = fopen(\"in.txt\", \"r\");\n     if (f == NULL) break;     \n  }\n  while(0);\n\n}\n
`;

sample["c2y"]["defer-with-return-v"] =
`
\n#include <stdio.h>\n\nint main()\n{\n  FILE* f = fopen(\"in.txt\", \"r\");\n  if (f == NULL) return 1;\n  _Defer fclose(f);\n\n  FILE* f2 = fopen(\"out.txt\", \"w\");\n  if (f2 == NULL) return 1;\n  _Defer fclose(f2);\n\n  return 0;\n}\n
`;

sample["c2y"]["elvis-operator"] =
`
int f(int x);\n\nint test_simple(int a, int b) {\n    return a ?: b;\n}\n\nint * test_pointer(int * p, int * fallback) {\n    return p ?: fallback;\n}\n\nint test_side_effect(int i, int b) {\n\n    return i++ ?: b;\n}\n\nint test_call(int b) {\n    return f(1) ?: b;\n}\nint test_nested(int a, int b, int c) {\n    return a ?: (b ? b : c);\n}\n\nint test_return(int x) {\n    return x ?: 42;\n}\n\nstatic_assert( 1?: 0 == 1);\n
`;

sample["c2y"]["generic-functions"] =
`
\n#define SWAP(a, b)  (static void (typeof(a) arg1, typeof(b) arg2)) {     typeof(*a) temp = *arg1;    *arg1 = *arg2;    *arg2 = temp;   }(a, b)\n\nint main()\n{\n    int a = 1;\n    int b = 2;\n    SWAP(&a, &b);\n    SWAP(&a, &b);\n}\n
`;

sample["c2y"]["generic-type-name"] =
`
/*\n  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3214.pdf\n*/\n\nint main(void)\n{\n    const int * const p = nullptr;\n    static_assert(_Generic(p, const int * : 1 ));\n    static_assert(_Generic(typeof(p), const int * const: 1));\n  \n    static_assert(_Generic(int, int : 1));    \n    static_assert(_Generic(typeof(\"abc\"), char [4]: 1));\n}\n
`;

sample["c2y"]["if-declarations"] =
`
/*\n  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3356.htm\n  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3580.htm\n*/\n\n#include <stdio.h>\n\nint main()\n{\n   \n   FILE* f0;\n   if ( f0 = fopen(\"file.txt\", \"r\"))\n   {\n     /*...*/\n     fclose(f0);\n   }\n   \n   if (FILE* f = fopen(\"file.txt\", \"r\"); f)\n   {\n     /*...*/\n     fclose(f);\n   }\n\n   if (FILE* f = fopen(\"file.txt\", \"r\"))\n   {    \n     /*...*/\n     fclose(f);\n   }\n}\n
`;

sample["c2y"]["literal-function-1"] =
`
#include <stdio.h>\nint main()\n{\n  printf(\"%d\", (static int (void) ){\n    return 1;\n  }());\n}\n
`;

sample["c2y"]["literal-function-async-i"] =
`
#include <stdlib.h>\n#include <stdio.h>\n\nvoid async(void * capture, void callback(void* capture, int result))\n{\n   callback(capture, 1);\n}\n\nint main()\n{\n\tstruct capture {int value; }* capture = calloc(1, sizeof * capture);\n    if (capture == 0) return 1;\n\n    capture->value = 123;\n    async(capture, (static void (void * capture, int result))\n    {\n\t\tstruct capture* p = capture;\n        printf(\"result=%d, value=%d\\n\", result, p->value);\n        free(p);\n    });\n}\n
`;

sample["c2y"]["literal-function-async-ii"] =
`
/*\n   Pattern:\n   do this -> then that -> then that ....\n*/\n\n#include <stdlib.h>\n#include <stdio.h>\n\nvoid login_async(void * data, void callback(void* data, int id))\n{\n   callback(data, 1);\n}\n\nvoid get_data_async(void * data, void callback(const char* email, void * data))\n{\n  callback(data, \"your data...\");\n}\n\nint main()\n{\n\tstruct capture { int id; }* capture = calloc(1, sizeof * capture);\n    login_async(capture, (static void (int id, void * capture))\n    {\n        struct capture * cap1 = capture;\n\n        printf(\"login completed. id=%d\\n\", id);\n        cap1->id = id;\n        get_data_async(cap1, (static void (const char* email, void * data))\n        {\n\t\t    struct capture * cap2 = data;\n            printf(\"your data='%s'  from id=%d\\n\", email, cap2->id);\n            free(cap2);\n        });\n    });\n}\n
`;

sample["c2y"]["literal-function-scopes"] =
`
\nvoid f1(){\n    /*we cannot use local variables*/\n    int i = 0;\n    //(void(void)){ i = 1; }();\n}\n\nvoid f2(){\n    /*we can use then at discarded expressions*/\n    int i = 0;\n    (static void (void)){ int k = sizeof(i); }();\n}\n\nint g;\nvoid f3(){\n    /*we can use variables from file scope*/\n    (static void(void)){ int k = g; }();\n}\n\n\nvoid f4(){\n    (static void(void)){ const char * s = __func__; }();\n}\n
`;

sample["c2y"]["local-functions-i"] =
`
int main()\n{\n\tstatic int dup(int a) { return a * 2; }\n    return dup(1);\n}\n
`;

sample["c2y"]["local-functions-ii"] =
`
#include <stdlib.h>\n\nvoid async(void callback(int result, void * data), void * data);\n\nint main()\n{\n\tstruct {int value; }* capture = calloc(1, sizeof * capture);\n\n    static void callback(int result, void * data)\n    {\n\t\ttypeof(capture) p = data;\n        free(p);\n    }\n\n    async(callback, capture);\n}\n
`;

sample["c2y"]["local-functions-iii"] =
`
#include <stdio.h>\n\nvoid f() {\n    printf(\"extern function\\n\");\n}\n\nint main()\n{    \n    static void f(); //forward declaration\n\n    static void f() {\n        printf(\"local function\\n\");\n    }\n\n    f(); //calls local function\n}\n
`;

sample["c2y"]["new-0o-and-0o-prefixes-for-octal-literals"] =
`
 //Obsolete implicitly octal literals\n //https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3319.htm\n\nstatic_assert(0o52 == 052);\nstatic_assert(0O52 == 052);\nstatic_assert(0O52 == 42);\n\nint main()\n{\n    int i = 0o52;\n}\n
`;

sample["c2y"]["statement-expressions"] =
`
\n//https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3643.htm\n\n#include <stdio.h>\n\n#define maxint(a,b)   ({int _a = (a), _b = (b); _a > _b ? _a : _b; })\n\nint main()\n{\n  printf(\"%d\", maxint(1, 2));\n}\n
`;

sample["c2y"]["static-assertions-in-expressions"] =
`
//https://open-std.org/jtc1/sc22/wg14/www/docs/n3715.pdf\n\n#include <stdio.h>\n\n#define BIT(n) ( \\\n  static_assert(n >= 0), \\\n  static_assert(n < 32), \\\n  1U << (n) \\\n)\n\nint main()\n{\n    static_assert(1);\n    printf(\"%u\", BIT(1));\n    return 0;\n}\n
`;

sample["c89"] = [];

sample["c89"]["bit-fields"] =
`
struct X {\n    unsigned char a : 3;\n    unsigned int b : 9;\n};\n\n#ifdef _MSC_VER\n static_assert(sizeof(struct X) == 8);\n static_assert(alignof(struct X) == 4);\n#else\n static_assert(sizeof(struct X) == 4);\n static_assert(alignof(struct X) == 4);\n#endif\n\nint main() {}\n
`;

sample["c89"]["enuns"] =
`
enum escapes { BELL = '\\a', BACKSPACE = '\\b', TAB = '\\t',\n               NEWLINE = '\\n', VTAB = '\\v', RETURN = '\\r' };\n\nenum months { JAN = 1, FEB, MAR, APR, MAY, JUN,\n              JUL, AUG, SEP, OCT, NOV, DEC };\n              /* FEB is 2, MAR is 3, etc. */\nint main()\n{\n    enum months m  = FEB;\n}\n
`;

sample["c89"]["x86_msvc\\bit-fields"] =
`
/* Cake 0.14.05 x86_msvc 2026-07-13 14:29:39 */\n\nint main()\n{\n}\n
`;

sample["c89"]["x86_msvc\\enuns"] =
`
/* Cake 0.14.05 x86_msvc 2026-07-13 14:29:34 */\n\nint main()\n{\n    int m;\n\n    m = 2;\n}\n
`;

sample["c99"] = [];

sample["c99"]["bool"] =
`
\nint main(void)\n{\n    _Bool b0 = false;\n    _Bool b1 = true;\n    _Bool b2 = 123;\n\n    b1 = b0;\n    b2 = 1234;\n    return 0;\n}\n\nvoid f1(_Bool b, int i){}\n\n_Bool f2(int i, int j)\n{\n    _Bool b = i;\n\n    f1(i, i);\n    f1(i = j, i = j);\n    f1(1 ? i : j, 1 ? i : j);\n\n    b = i;\n    b = i = j;\n    b = i = j && i;\n    b = 1 ? i : j;\n    b = 1, i;\n    return i;\n}\n
`;

sample["c99"]["complex"] =
`
   /*not implemented*/\n
`;

sample["c99"]["compound-literals"] =
`
struct X {\n  int i;\n};\n\nint main(){\n   struct X x = {};\n   x = (struct X){};\n}\n
`;

sample["c99"]["designated-initializers"] =
`
\n\nstruct T {\n    int k;\n    int l;\n};\n\nstruct S {\n    int i;\n    struct T t;\n};\n\nstruct T x = {\n    .l = 43,\n    .k = 42,\n};\n\nvoid f(void) {\n    struct S l = {\n        1,\n        .t = x,\n        .t.l = 41,\n    };\n}\n\nint main() {\n    int a[6] = {[4] = 29, [2] = 15};\n\n    struct point {\n        int x, y;\n    };\n    struct point p = {.y = 2, .x = 3};\n\n    struct {\n        int a[3], b;\n    } w[] = {[0].a = {1}, [1].a[0] = 2};\n}\n\n#define A_MAX 20\nint a[A_MAX] = { 1, 3, 5, 7, 9, [A_MAX-5] = 8, 6, 4, 2, 0 };\n
`;

sample["c99"]["flexible-array-members"] =
`
\n#include <stdio.h>\n#include <stdlib.h>\n\nstruct X {\n    int count;\n    double values[]; // flexible array\n};\n\n/*\n    The size of a structure with a flexible array member is\n    determined as if the flexible array member were omitted,\n    EXCEPT that it may have more trailing padding than the\n    omission would imply\n*/\n\nint main() {\n\n    int n = 3;\n\n    printf(\"sizeof(struct X) = %d\\n\", (int) sizeof(struct X));\n    printf(\"allocated = %d\\n\", (int) sizeof(struct X) + n * sizeof(double));\n\n    struct X* p = malloc(sizeof(struct X) + n * sizeof(double));\n    if (p == NULL) return 0;\n\n    p->count = n;\n    p->values[0] = 10.0;\n    p->values[1] = 20.0;\n    p->values[2] = 30.0;\n\n    for (int i = 0; i < p->count; ++i)\n        printf(\"%f\\n\", p->values[i]);\n\n    free(p);\n\n    return 0;\n}\n
`;

sample["c99"]["func"] =
`
#include <stdio.h>\nint main()\n{\n    printf(\"%s\\n\", __func__);\n    printf(\"%s\\n\", __func__);\n}\n
`;

sample["c99"]["hexadecimal-floating-constants"] =
`
const double d = 0x1p+1;\nconst double dmax = 0x1.fffffffffffffp+1023;\nconst double dmin = 0x1p-1022;\n\n/*\n  Note : The result may not be so precise as the original\n*/\n
`;

sample["c99"]["init-clause-of-the-for-loop"] =
`
int main()\n{\n   const int max = 10;\n   for (int n = max - 1; n >= 0; n--)\n   {\n     // body of loop\n   }\n}\n
`;

sample["c99"]["inline"] =
`
inline int sum(int a, int b)\n{\n    return a + b;\n}\n\nint main()\n{\n    int r = sum(1,3);\n}\n
`;

sample["c99"]["int-a-static"] =
`
#include <stdlib.h>\n\n\nvoid F(int a[static const 5]) \n{\n    //cake was a warning when modifying array arguments\n    a = 0;\n\n    //bounds check for constant indexes\n    int k = a[5];\n}\n\n\nint main() {\n    \n    F(0);\n    F(NULL);\n    F(nullptr);\n\n    int a[] = {1, 2, 3};    \n    //F(a); /*try*/\n    \n    int b[] = { 1, 2, 3 , 4, 5};\n    F(b);\n    int k = b[5]; //bounds check\n\n    int c[] = { 1, 2, 3 , 4, 5, 6};\n    F(c);\n}\n
`;

sample["c99"]["mixed-declarations"] =
`
int main(){\n  int x = 10;\n  x++;\n  int y = 20; // allowed in C99\n}\n
`;

sample["c99"]["pragma"] =
`
//6.10.11 Pragma operator\n\n#define LISTING(x) PRAGMA(listing on #x)\n#define PRAGMA(x) _Pragma(#x)\n\nLISTING (..listing.dir)\n
`;

sample["c99"]["restrict-pointers"] =
`
   void copy_array(int n, int *restrict a, int *restrict b)\n   {\n      while (n-- > 0)\n        *a++ = *b++;\n   }\n
`;

sample["c99"]["sizeof-vmt"] =
`
\n#include <stdio.h>\n\nvoid sizeof_sample()\n{\n    int n = 1;\n    printf(\"%zu \", sizeof(int [n++]));\n\n    printf(\"%zu \", sizeof(int [n++]));\n\n    printf(\"%zu \", sizeof(int [n++]));\n    \n    printf(\"\\n\");\n}\n\nvoid countof_sample()\n{\n    int n = 1;\n    printf(\"%zu \", _Countof(int [n++]));\n\n    printf(\"%zu \", _Countof(int [n++]));\n\n    printf(\"%zu \", _Countof(int [n++]));\n    \n    printf(\"\\n\");\n}\n\nint main(){\n    sizeof_sample();\n    countof_sample();\n}\n\n// 4 8 12 \n// 1 2 3\n
`;

sample["c99"]["typeof-vmt"] =
`
#include <stdio.h>\nint main()\n{\n    int n = 1;\n    typeof(int (*)[n++]) p1;\n    typeof(int (*)[n++]) p2;\n    typeof(int (*)[n++]) p3;\n    \n    printf(\"%zu \", _Countof(*p1));\n\n    printf(\"%zu \", _Countof(*p2));\n\n    printf(\"%zu \", _Countof(*p3));\n    \n    printf(\"\\n\");\n}\n//1 2 3\n
`;

sample["c99"]["va-args"] =
`
#include <stdio.h>\n\n#define MYLOG(FormatLiteral, ...)  fprintf (stderr, \"%s(%u): \" FormatLiteral \"\\n\", __FILE__, __LINE__, __VA_ARGS__)\n\nint main()\n{\n MYLOG(\"Too many balloons %u\", 42);\n}\n
`;

sample["c99"]["vla-i"] =
`
#include <stdio.h>\n\nint main(void) {\n    int a = 3, b = 4;\n\n    int size = a * b; // runtime expression\n    int arr[size];    // VLA\n\n    for (int i = 0; i < size; i++) {\n        arr[i] = i;\n    }\n\n    printf(\"Size = %d\\n\", size);\n    for (int i = 0; i < size; i++) {\n        printf(\"%d \", arr[i]);\n    }\n    printf(\"\\n\");\n\n    return 0;\n}\n
`;

sample["c99"]["vla-ii"] =
`
/*\n  VLA inside a block (lifetime demonstration)\n*/\n\n#include <stdio.h>\n\nint main(void) {\n    int n = 3;\n\n    printf(\"Before block\\n\");\n\n    {\n        int arr[n]; // VLA with block scope\n\n        for (int i = 0; i < n; i++) {\n            arr[i] = i * i;\n        }\n\n        printf(\"Inside block:\\n\");\n        for (int i = 0; i < n; i++) {\n            printf(\"%d \", arr[i]);\n        }\n        printf(\"\\n\");\n    }\n\n    printf(\"After block\\n\");\n    return 0;\n}\n
`;

sample["c99"]["vla-iii"] =
`
/*\nVLA with 2D function parameter\n*/\n#include <stdio.h>\n\nvoid print_matrix(int rows, int cols, int m[rows][cols]) {\n    for (int i = 0; i < rows; i++) {\n        for (int j = 0; j < cols; j++) {\n            printf(\"%d \", m[i][j]);\n        }\n        printf(\"\\n\");\n    }\n}\n\nint main(void) {\n    int r = 2, c = 3;\n\n    int m[r][c]; // VLA\n\n    for (int i = 0; i < r; i++)\n        for (int j = 0; j < c; j++)\n            m[i][j] = i * c + j;\n\n    print_matrix(r, c, m);\n    return 0;\n}\n
`;

sample["c99"]["vm-types-ii"] =
`
#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n\n\nvoid test_3d(int x, int y, int z)\n{\n    int (*p)[x][y][z] = malloc(sizeof *p);\n    int i, j, k;\n    for (i = 0; i < x; i++)\n        for (j = 0; j < y; j++)\n            for (k = 0; k < z; k++)\n                (*p)[i][j][k] = i * 100 + j * 10 + k;\n\n    printf(\"p[1][2][1] = %d\\n\", (*p)[1][2][1]); /* expect 121 */\n    printf(\"p[2][0][3] = %d\\n\", (*p)[2][0][3]); /* expect 203 */\n    free(p);\n}\n\n\nstruct Point { int x; int y; };\n\nvoid test_struct_array(int n)\n{\n    struct Point (*pts)[n] = malloc(sizeof *pts);\n    int i;\n    for (i = 0; i < n; i++)\n    {\n        (*pts)[i].x = i;\n        (*pts)[i].y = i * 2;\n    }\n    for (i = 0; i < n; i++)\n        printf(\"(%d,%d) \", (*pts)[i].x, (*pts)[i].y);\n    printf(\"\\n\");\n    free(pts);\n}\n\n\nvoid zero_row(int m, int (*row)[m])\n{\n    int j;\n    for (j = 0; j < m; j++)\n        (*row)[j] = 0;\n}\n\nvoid set_diagonal(int n, int m, int (*mat)[n][m])\n{\n    int i;\n    for (i = 0; i < n && i < m; i++)\n        (*mat)[i][i] = 1;\n}\n\nvoid test_call_chain(int n, int m)\n{\n    int (*mat)[n][m] = malloc(sizeof *mat);\n    int i, j;\n    /* zero everything */\n    for (i = 0; i < n; i++)\n        for (j = 0; j < m; j++)\n            (*mat)[i][j] = 0;\n\n    set_diagonal(n, m, mat);\n\n    for (i = 0; i < n; i++)\n    {\n        for (j = 0; j < m; j++)\n            printf(\"%d \", (*mat)[i][j]);\n        printf(\"\\n\");\n    }\n    free(mat);\n}\n\n\nvoid test_jagged(int rows, int cols[rows])\n{\n    int i, j;\n\n    /* array of pointers, each pointing to a VM row */\n    int **jagged = malloc(sizeof(int*) * rows);\n    for (i = 0; i < rows; i++)\n    {\n        int (*row)[cols[i]] = malloc(sizeof *row);\n        for (j = 0; j < cols[i]; j++)\n            (*row)[j] = i * 10 + j;\n        jagged[i] = (int*)row;\n    }\n\n    for (i = 0; i < rows; i++)\n    {\n        for (j = 0; j < cols[i]; j++)\n            printf(\"%3d\", jagged[i][j]);\n        free(jagged[i]);\n        printf(\"\\n\");\n    }\n    free(jagged);\n}\n\n\nvoid* alloc_matrix(int n, int m)\n{\n    int (*p)[n][m] = malloc(sizeof *p);\n    return p;\n}\n\nvoid test_return_vm(int n, int m)\n{\n    int (*mat)[n][m] = alloc_matrix(n, m);\n    int i, j;\n    for (i = 0; i < n; i++)\n        for (j = 0; j < m; j++)\n            (*mat)[i][j] = (i + 1) * (j + 1);\n    for (i = 0; i < n; i++)\n    {\n        for (j = 0; j < m; j++)\n            printf(\"%4d\", (*mat)[i][j]);\n        printf(\"\\n\");\n    }\n    free(mat);\n}\n\nvoid test_sizeof_in_expr(int n, int m)\n{\n    int (*a)[n][m] = malloc(sizeof *a);\n    int (*b)[n][m] = malloc(sizeof *b);\n\n    int i, j;\n    for (i = 0; i < n; i++)\n        for (j = 0; j < m; j++)\n            (*a)[i][j] = i * m + j + 1;\n\n    /* memcpy using sizeof *a */\n    memcpy(b, a, sizeof *a);\n\n    /* memset using sizeof *b */\n    memset(a, 0, sizeof *a);\n\n    printf(\"a[0][0] after memset = %d\\n\",  (*a)[0][0]); /* 0 */\n    printf(\"b[0][0] after memcpy = %d\\n\",  (*b)[0][0]); /* 1 */\n    printf(\"b[n-1][m-1] = %d\\n\", (*b)[n-1][m-1]);       /* n*m */\n    free(a);\n    free(b);\n}\n\n\nvoid test_conditional_size(int use_large)\n{\n    int n = use_large ? 10 : 3;\n    int m = use_large ? 10 : 3;\n    int (*p)[n][m] = malloc(sizeof *p);\n    int i, j;\n    for (i = 0; i < n; i++)\n        for (j = 0; j < m; j++)\n            (*p)[i][j] = i * m + j;\n    printf(\"p[%d][%d] = %d\\n\", n-1, m-1, (*p)[n-1][m-1]);\n    printf(\"sizeof = %zu\\n\", sizeof(*p));\n    free(p);\n}\n\n\nvoid scale(int n, int m, int (*mat)[n][m], int factor)\n{\n    int i, j;\n    for (i = 0; i < n; i++)\n        for (j = 0; j < m; j++)\n            (*mat)[i][j] *= factor;\n}\n\nvoid add_matrices(int n, int m,\n                  int (*dst)[n][m],\n                  int (*src)[n][m])\n{\n    int i, j;\n    for (i = 0; i < n; i++)\n        for (j = 0; j < m; j++)\n            (*dst)[i][j] += (*src)[i][j];\n}\n\nvoid test_matrix_ops(int n, int m)\n{\n    int (*a)[n][m] = malloc(sizeof *a);\n    int (*b)[n][m] = malloc(sizeof *b);\n    int i, j;\n\n    for (i = 0; i < n; i++)\n        for (j = 0; j < m; j++)\n        {\n            (*a)[i][j] = i * m + j + 1;\n            (*b)[i][j] = 1;\n        }\n\n    scale(n, m, a, 2);\n    add_matrices(n, m, a, b);\n\n    /* a[i][j] = (i*m+j+1)*2 + 1 */\n    printf(\"a[0][0]=%d\\n\", (*a)[0][0]); /* (0+0+1)*2+1 = 3 */\n    printf(\"a[1][1]=%d\\n\", (*a)[1][1]); /* (m+2)*2+1   */\n    free(a);\n    free(b);\n}\n\n\nvoid test_ptr_arith(int n)\n{\n    int (*p)[n] = malloc(sizeof(int) * n * 3);\n    int (*q)[n];\n    int i;\n\n    /* fill 3 rows */\n    for (i = 0; i < n * 3; i++)\n        ((int*)p)[i] = i;\n\n    /* advance pointer by one \"row\" of n ints */\n    q = p + 1;\n\n    printf(\"p[0]=%d q[0]=%d\\n\", (*p)[0], (*q)[0]); /* 0, n */\n    free(p);\n}\n\nvoid test_vm_typedef(int n, int m)\n{\n    /* C99: typedef int Row[m]; Row (*mat)[n] = malloc(...) */\n    /* Equivalent without typedef: */\n    int (*mat)[n][m] = malloc(sizeof *mat);\n    int i, j;\n    for (i = 0; i < n; i++)\n        for (j = 0; j < m; j++)\n            (*mat)[i][j] = (i == j) ? 1 : 0;  /* identity */\n\n    for (i = 0; i < n; i++)\n    {\n        for (j = 0; j < m; j++)\n            printf(\"%d \", (*mat)[i][j]);\n        printf(\"\\n\");\n    }\n    free(mat);\n}\n\nint main(void)\n{\n    printf(\"=== 1. 3-D VM pointer ===\\n\");\n    test_3d(3, 3, 4);\n\n    printf(\"=== 2. VM pointer to struct array ===\\n\");\n    test_struct_array(4);\n\n    printf(\"=== 3. VM through call chain ===\\n\");\n    test_call_chain(3, 4);\n\n    printf(\"=== 4. Jagged array via VM rows ===\\n\");\n    int cols[4] = {2, 4, 1, 3};\n    test_jagged(4, cols);\n\n    printf(\"=== 5. VM via void* return ===\\n\");\n    test_return_vm(3, 4);\n\n    printf(\"=== 6. sizeof in memcpy/memset ===\\n\");\n    test_sizeof_in_expr(3, 4);\n\n    printf(\"=== 7. Conditional allocation ===\\n\");\n    test_conditional_size(0);\n    test_conditional_size(1);\n\n    printf(\"=== 8. Matrix ops with multiple VM params ===\\n\");\n    test_matrix_ops(2, 3);\n\n    printf(\"=== 9. VM pointer arithmetic ===\\n\");\n    test_ptr_arith(4);\n\n    printf(\"=== 10. VM typedef equivalent ===\\n\");\n    test_vm_typedef(3, 3);\n\n    return 0;\n}\n
`;

sample["c99"]["vm-types"] =
`
#include <stdio.h>\n#include <stdlib.h>\n\nvoid print2D(int n, int m, int a[n][m])\n{\n    for (int i = 0; i < n; i++)\n    {\n        for (int j = 0; j < m; j++)\n        {\n            printf(\"%d \", a[i][j]);\n        }\n        printf(\"\\n\");\n    }\n}\n\nvoid print1D(int n, int a[n])\n{\n    for (int i = 0; i < n; i++)\n    {\n        printf(\" %d\", a[i]);        \n    }\n    printf(\"\\n\");\n}\n\nvoid test_1d(int n)\n{\n    int (*p)[n] = malloc(sizeof *p);\n    int i;\n    for (i = 0; i < n; i++)\n        (*p)[i] = i + 1;\n    for (i = 0; i < n; i++)\n        printf(\"%d \", (*p)[i]);\n    printf(\"\\n\");\n    free(p);\n}\n\nvoid test_2d(int n, int m)\n{\n    int (*p)[n][m] = malloc(sizeof *p);\n    int i, j;\n    for (i = 0; i < n; i++)\n        for (j = 0; j < m; j++)\n            (*p)[i][j] = i * m + j;\n    for (i = 0; i < n; i++)\n    {\n        for (j = 0; j < m; j++)\n            printf(\"%3d\", (*p)[i][j]);\n        printf(\"\\n\");\n    }\n    free(p);\n}\n\nvoid test_sizeof_stable(int n, int m)\n{\n    int (*p)[n][m] = malloc(sizeof *p);\n    size_t sz = sizeof(*p);\n    printf(\"sizeof(*p) = %zu\\n\", sz);\n    n = 999;\n    m = 999;\n    printf(\"sizeof(*p) after n=999,m=999 = %zu\\n\", sizeof(*p));\n    printf(\"same? %d\\n\", sz == sizeof(*p));\n    free(p);\n}\n\nvoid fill(int n, int m, int (*grid)[n][m])\n{\n    int i, j;\n    for (i = 0; i < n; i++)\n        for (j = 0; j < m; j++)\n            (*grid)[i][j] = i * 10 + j;\n}\n\nvoid print_grid(int n, int m, int (*grid)[n][m])\n{\n    int i, j;\n    for (i = 0; i < n; i++)\n    {\n        for (j = 0; j < m; j++)\n            printf(\"%3d\", (*grid)[i][j]);\n        printf(\"\\n\");\n    }\n}\n\nvoid test_vm_param(int n, int m)\n{\n    int (*p)[n][m] = malloc(sizeof *p);\n    fill(n, m, p);\n    print_grid(n, m, p);\n    free(p);\n}\n\nvoid test_multiple(int n, int m)\n{\n    int (*a)[n] = malloc(sizeof *a);\n    int (*b)[m] = malloc(sizeof *b);\n    int i;\n    for (i = 0; i < n; i++) (*a)[i] = i;\n    for (i = 0; i < m; i++) (*b)[i] = i + 100;\n    printf(\"a[0]=%d a[n-1]=%d\\n\", (*a)[0], (*a)[n - 1]);\n    printf(\"b[0]=%d b[m-1]=%d\\n\", (*b)[0], (*b)[m - 1]);\n    free(a);\n    free(b);\n}\n\nvoid test_mixed_dims(int n)\n{\n    int (*p)[4][n] = malloc(sizeof *p);\n    int i, j;\n    for (i = 0; i < 4; i++)\n        for (j = 0; j < n; j++)\n            (*p)[i][j] = i * n + j;\n    for (i = 0; i < 4; i++)\n    {\n        for (j = 0; j < n; j++)\n            printf(\"%3d\", (*p)[i][j]);\n        printf(\"\\n\");\n    }\n    free(p);\n}\n\nvoid test_reassign(int n)\n{\n    int (*p)[n] = malloc(sizeof *p);\n    printf(\"sizeof(*p) first  = %zu\\n\", sizeof(*p));\n    free(p);\n    n = 2;                         /* n changes */\n    p = malloc(sizeof *p);         /* new allocation — but snapshot is fixed */\n    printf(\"sizeof(*p) second = %zu\\n\", sizeof(*p));\n    free(p);\n}\n\nvoid typedef_sample()\n{\n    int n = 1;\n    typedef int (*T)[n];\n    n = 2;\n    T b;\n    printf(\"%zu\\n\", sizeof(*b)); // prints 4\n\n    n = 3;\n    T c;\n    printf(\"%zu\\n\", sizeof(*c)); //prints 4\n}\n\nint main(void)\n{\n    test_1d(5);\n\n    test_2d(3, 4);\n\n    test_sizeof_stable(3, 4);\n\n    test_vm_param(3, 4);\n\n    test_multiple(4, 3);\n\n    test_mixed_dims(3);\n\n    test_reassign(5);\n\n    typedef_sample();\n\n    return 0;\n}\n
`;

sample["c99"]["vmt-cast"] =
`
#include <stdio.h>\nvoid test()\n{\n   int n = 1;\n   typeof(int(*)[n++]) a;\n   (int(*)[n++]) 0;\n   (int(*)[n++]) 0;\n   printf(\"%d\", n);\n}\n//4\n\nint main()\n{\n   test();\n\n   int n = 1;\n   int a[n];\n    n = 2;\n   auto p = (int(*)[n]) & a;\n   sizeof(* p); //8\n}\n
`;

sample["cross-compiling"] = [];

sample["cross-compiling"]["integers"] =
`
\n\nauto i32_max = 2'147'483'647;\nauto i32_max1 = 2'147'483'648;\n\nauto u32_max = 4'294'967'295;\nauto u32_max1 = 4'294'967'296;\n\nauto i64_max = 9'223'372'036'854'775'807;\nauto i64_max1 = 9'223'372'036'854'775'808;\nauto u64_max = 18'446'744'073'709'551'615;\n
`;

sample["cross-compiling"]["long"] =
`
\n#include <stdio.h>\nint main(void)\n{\n    printf(\"value=%ld\", 2147483647L + 10);\n\n    #ifdef _WIN32\n    //use option : -target=x86_msvc\n    static_assert(-2147483639 == 2147483647L + 10, \"\");\n    #else\n    //use option : -target=x86_x64_gcc\n    static_assert(2147483657LL == 2147483647L + 10, \"\");\n    #endif\n\n    auto i32_max = 2'147'483'647;\n    auto u32_max = 4'294'967'295;\n    auto i64_max = 9'223'372'036'854'775'807;\n    auto u64_max = 18'446'744'073'709'551'615;\n\n}\n
`;

sample["cross-compiling"]["target"] =
`
/*\n   Cake is a cross-compiling compiler, but unlike traditional compilers\n   that produce machine executables directly, Cake generates C code as\n   its output. This C code is tailored for the target platform, taking\n   into account differences in architecture, data sizes, and calling conventions.\n*/\n\n#ifdef _WIN64\n    // 64-bit Windows  -target=x64_msvc\n\n    //long is 4 bytes on windows\n    static_assert(sizeof(1L) == 4);\n    static_assert(sizeof(void*) == 8);\n    static_assert(sizeof(L\"\"[0]) == 2);\n\n#elif defined _WIN32\n\nstatic_assert(sizeof(bool) == 1);\nstatic_assert(alignof(bool) == 1);\n\nstatic_assert(sizeof(char) == 1);\nstatic_assert(alignof(char) == 1);\n\nstatic_assert(sizeof(short) == 2);\nstatic_assert(alignof(short) == 2);\n\nstatic_assert(sizeof(L' ') == 2);\nstatic_assert(sizeof(u' ') == 2);\nstatic_assert(sizeof(U' ') == 4);\n\nstatic_assert(sizeof(int) == 4);\nstatic_assert(alignof(int) == 4);\n\nstatic_assert(sizeof(long) == 4);\nstatic_assert(alignof(long) == 4);\n\nstatic_assert(sizeof(long long) == 8);\nstatic_assert(alignof(long long) == 8);\n\nstatic_assert(sizeof(float) == 4);\nstatic_assert(alignof(float) == 4);\n\nstatic_assert(sizeof(double) == 8);\nstatic_assert(alignof(double) == 8);\n\nstatic_assert(sizeof(long double) == 8);\nstatic_assert(alignof(long double) == 8);\n\nstatic_assert(sizeof(void*) == 4);\nstatic_assert(alignof(void*) == 4);\n\nstatic_assert(sizeof(nullptr) == 4);\nstatic_assert(alignof(nullptr) == 4);\n\nstatic_assert(sizeof(sizeof(0)) == 4);\nstatic_assert(alignof(sizeof(0)) == 4);\n\nstatic_assert(sizeof((char*)0-1) == 4);\nstatic_assert(alignof((char*)0-1) == 4);\n\n\n\n#endif\n\n#ifdef __x86_64__\n\n// linux -target=x86_x64_gcc\n\n_Static_assert(sizeof(bool) == 1, \"\");\n_Static_assert(alignof(bool) == 1, \"\");\n\n_Static_assert(sizeof(char) == 1, \"\");\n_Static_assert(alignof(char) == 1, \"\");\n\n_Static_assert(sizeof(short) == 2, \"\");\n_Static_assert(alignof(short) == 2, \"\");\n\n_Static_assert(sizeof(L' ') == 4, \"\");\n_Static_assert(sizeof(u' ') == 2, \"\");\n_Static_assert(sizeof(U' ') == 4, \"\");\n\n_Static_assert(sizeof(int) == 4, \"\");\n_Static_assert(alignof(int) == 4, \"\");\n\n_Static_assert(sizeof(long) == 8, \"\");\n_Static_assert(alignof(long) == 8, \"\");\n\n_Static_assert(sizeof(long long) == 8, \"\");\n_Static_assert(alignof(long long) == 8, \"\");\n\n_Static_assert(sizeof(float) == 4, \"\");\n_Static_assert(alignof(float) == 4, \"\");\n\n_Static_assert(sizeof(double) == 8, \"\");\n_Static_assert(alignof(double) == 8, \"\");\n\n_Static_assert(sizeof(long double) == 16, \"\");\n_Static_assert(alignof(long double) == 16, \"\");\n\n_Static_assert(sizeof(void*) == 8, \"\");\n_Static_assert(alignof(void*) == 8, \"\");\n\n_Static_assert(sizeof(nullptr) == 8, \"\");\n_Static_assert(alignof(nullptr) == 8, \"\");\n\n_Static_assert(sizeof(sizeof(0)) == 8, \"\");\n_Static_assert(alignof(sizeof(0)) == 8, \"\");\n\n_Static_assert(sizeof((char*)0 - 1) == 8, \"\");\n_Static_assert(alignof((char*)0 - 1) == 8, \"\");\n\n\n#endif\n
`;

sample["extensions"] = [];

sample["extensions"]["assert"] =
`
\nint main()\n{\n   _Assert(1 == 1);\n}\n\n
`;

sample["extensions"]["attribute"] =
`
   //GCC __attributes__ are parsed.\n   //Cake implements C23 attributes and some GCC attributes could be\n   //mapped but they are not at this moment.\n
`;

sample["extensions"]["builtin-c23-va-start"] =
`
typedef __builtin_va_list va_list;\n\nint add_nums_C23(int count, ...)\n{\n    int result = 0;\n    va_list args;\n    __builtin_c23_va_start(args);\n\n\n    for (int i = 0; i < count; ++i) {\n        result += __builtin_va_arg(args, int);\n    }\n\n    __builtin_va_end(args);\n    return result;\n}\n\nint main(void)\n{\n    add_nums_C23(4 /*count*/, 25, 25, 50, 50);\n}\n
`;

sample["extensions"]["builtin-offsetof"] =
`
\nstruct S {\n    char c;\n    double d;\n};\n\nint main(void)\n{\n    static_assert(__builtin_offsetof(struct S, c) == 0);\n    static_assert(__builtin_offsetof(struct S, d) == 8);\n}\n
`;

sample["extensions"]["checked-expressions-i"] =
`
#include <stdio.h>\n\nint main()\n{\n  try\n  {\n     FILE* f = fopen(\"in.txt\", \"r\") !;\n     defer fclose(f);\n  }\n  catch\n  {\n  }\n}\n
`;

sample["extensions"]["checked-expressions-ii"] =
`
#pragma safety enable\n\nvoid* _Owner _Opt malloc(unsigned long size);\nvoid free(void* _Owner _Opt ptr);\n\nint main() {\n    try\n    {\n        int * _Owner p = malloc(1)!;\n        *p = 1;\n        free(p);\n    }\n    catch {\n\n    }\n}\n
`;

sample["extensions"]["consteval"] =
`
\n//syntax : const(constant-expression)\n// This would be more useful when cake evaluate functions\n// at compile time\n\nint main(void)\n{\n    int i = 1;\n    int j;\n    j = const(5 + 3);\n\n    // error\n    // j = const(i + 3);\n}\n
`;

sample["extensions"]["countof-enum"] =
`
#include <string.h>\nenum E { A, B, C, D, E, F };\n\nvoid f(enum E e)\n{\n    switch (e)\n    {\n\n        case A:\n            break;\n        case B:\n            break;\n        default:\n            static_assert(_Countof(e) == 6);\n    }\n}\n\nenum E parse_enum_e(const char* s)\n{\n    if (strcmp(s, \"A\") == 0) return A;\n    if (strcmp(s, \"B\") == 0) return B;\n    if (strcmp(s, \"C\") == 0) return C;\n    if (strcmp(s, \"D\") == 0) return D;\n    if (strcmp(s, \"E\") == 0) return E;\n    if (strcmp(s, \"F\") == 0) return F;\n    static_assert(_Countof(enum E) == 6);\n\n    return A;\n}\n\nint main() { }\n
`;

sample["extensions"]["line-slicing-checks"] =
`
\n#define M\\\nACRO 1\n\nint main()\n{\n    const char* s = \" asdas \\\n    asdas\";\n\n    int a = \\\n    1;\n    \n    //comment \\\n    a = 2;\n\n    /*\n      ok\n      path = c:\\path\\\n   */\n}\n
`;

sample["extensions"]["msvc-declspec"] =
`
   //MSVC __declspec are parsed.\n   //__declspec( selectany ) had to to implemented,\n   //because the output depends on it.\n
`;

sample["extensions"]["pragma-diagnostic"] =
`
/*\n  Diagnostic levels can be configured per code region using:\n    #pragma CAKE diagnostic push   (save current settings)\n    #pragma CAKE diagnostic error   <id> [id2 ...]\n    #pragma CAKE diagnostic warning <id> [id2 ...]\n    #pragma CAKE diagnostic note    <id> [id2 ...]\n    #pragma CAKE diagnostic ignored <id> [id2 ...]\n    #pragma CAKE diagnostic pop    (restore previous settings)\n\n  Warning 4 = mixing enumerators from different enum types.\n*/\n\nenum Color { RED, GREEN, BLUE };\nenum Direction { NORTH, SOUTH, EAST, WEST };\n\nint main()\n{\n    enum Color c = RED;\n    enum Direction d = NORTH;\n\n    /* Promote warning 4 to a hard error — mixing enums breaks the build */\n#pragma CAKE diagnostic push\n#pragma CAKE diagnostic error 4\n    if (c == SOUTH) {}   /* error: enumerators from different enums */\n#pragma CAKE diagnostic pop\n\n    /* Default level: warning */\n#pragma CAKE diagnostic push\n#pragma CAKE diagnostic warning 4\n    if (c == SOUTH) {}   /* warning: enumerators from different enums */\n#pragma CAKE diagnostic pop\n\n    /* Demote to a note — informational only */\n#pragma CAKE diagnostic push\n#pragma CAKE diagnostic note 4\n    if (c == SOUTH) {}   /* note: enumerators from different enums */\n#pragma CAKE diagnostic pop\n\n    /* Silence completely — useful for third-party or generated code */\n#pragma CAKE diagnostic push\n#pragma CAKE diagnostic ignored 4\n    if (c == SOUTH) {}   /* no diagnostic */\n#pragma CAKE diagnostic pop\n\n    /* Multiple warnings can be configured in one pragma */\n#pragma CAKE diagnostic push\n#pragma CAKE diagnostic ignored 4 10\n    if (c == SOUTH) {}   /* 4: enum mix — suppressed  */\n    c;                   /* 10: result not used — suppressed */\n#pragma CAKE diagnostic pop\n}\n
`;

sample["extensions"]["traits"] =
`
\nint main()\n{\n  int i;\n  static_assert(_is_integral(i));\n  static_assert(_is_floating_point(double) && _is_floating_point(float));\n  static_assert(_is_function(main));\n\n  char * p;\n  static_assert(_is_scalar(p));\n  static_assert(_is_scalar(nullptr));\n\n  int a[10];\n  static_assert(_is_array(a));\n\n  /*pf = pointer to function (void) returning array 10 of int*/\n  int (*pf)(void)[10];\n  static_assert(!_is_array(pf));\n  static_assert(_is_pointer(pf));\n\n  \n\n}\n
`;

sample["extensions"]["try-catch-throw-ii"] =
`
#include <stdio.h>\n\n/*not sure if usefull , but this is allowed*/\n\nint main()\n{\n  try\n  {\n      FILE * f = NULL;\n      try {\n         FILE *f = fopen(\"file.txt\", \"r\");\n         if (f == NULL) throw;\n         /*more*/\n      }\n      catch {\n         if (f)\n          fclose(f);\n         throw;\n      }\n  }\n  catch\n  {\n  }\n}\n
`;

sample["extensions"]["try-catch-throw"] =
`
#include <stdio.h>\n\nint main()\n{\n  FILE * f = NULL;\n  try\n  {\n     f = fopen(\"file.txt\", \"r\");\n     if (f == NULL) throw;\n\n    /*success here*/\n  }\n  catch\n  {\n     /*some error*/\n  }\n\n  if (f)\n    fclose(f);\n}\n
`;

sample["extensions"]["x86_msvc\\assert"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:57:02 */\n\nint main()\n{\n    1;\n}\n
`;

sample["find-the-bug"] = [];

sample["find-the-bug"]["bug-1"] =
`
#pragma safety enable\n\n#include <stdlib.h>\n#include <string.h>\n\nstruct X {\n  char *_Owner _Opt name;\n};\n\nstruct X f(int condition)\n{\n   struct X x;\n   if (condition) {\n        x.name = strdup(\"a\");\n   }\n   return x;\n}\n\nint main()\n{\n    struct X x = f(2);\n}\n
`;

sample["find-the-bug"]["bug-10"] =
`
#pragma safety enable\n\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n\nstruct person {\n    char* _Opt _Owner name;\n};\n\nvoid set(struct person* p, char* name) {\n    free(p->name);\n    char* _Opt _Owner temp = strdup(name);\n    if (temp == NULL) return;\n    p->name = temp;\n    /*may leave this function with p->name uninitialized!*/\n}\n\nint main() {\n    struct person p = {0};\n    set(&p, \"a\");\n    printf(\"%s\", p.name);\n    free(p.name);\n    return 0;\n}\n
`;

sample["find-the-bug"]["bug-11"] =
`
#pragma safety enable\n\nvoid* _Owner _Opt malloc(unsigned long size);\n\nstruct X{\n  char * p_ch;    \n};\n\nstruct X * _Owner _Opt f()\n{\n   char ch  = 0;\n   struct X * _Owner _Opt p = malloc(sizeof(struct X));\n   if (p)\n   {\n       p->p_ch =  &ch;\n       return p;\n   } \n   return nullptr;   \n}\n
`;

sample["find-the-bug"]["bug-2"] =
`
\n#pragma safety enable\n\n#include <stdlib.h>\n#include <string.h>\n\nstruct X {\n  char *_Owner _Opt name;\n};\n\nvoid delete(struct X * _Owner _Opt p)\n{\n    if (p)\n    {\n       /*forgot to free p->name*/\n       free(p);\n    }\n}\n\nint main()\n{\n    struct X * _Owner _Opt p = calloc(1, sizeof * p);\n    if (p)\n    {\n        p->name = strdup(\"a\");\n        delete(p);\n    }\n}\n
`;

sample["find-the-bug"]["bug-3"] =
`
#pragma safety enable\n\n#include <stdlib.h>\n#include <string.h>\n\nstruct X {\n  char *_Owner _Opt name;\n  char *_Owner _Opt surname;\n};\n\nvoid delete(struct X * _Owner _Opt p)\n{\n    if (p)\n    {\n       free(p->name);\n       /*forgot to free p->surname*/\n       free(p);\n    }\n}\n\nint main()\n{\n    struct X * _Owner _Opt p = malloc(sizeof * p);\n    if (p)\n    {\n        p->name = strdup(\"a\");\n        /*p->surname is uninitialized*/\n        delete(p);\n    }\n}\n
`;

sample["find-the-bug"]["bug-4"] =
`
\n#pragma safety enable\n\n#include <stdlib.h>\n#include <stdio.h>\n#include <string.h>\n\nstruct X {\n  char *_Owner _Opt name;\n  char *_Owner _Opt surname;\n};\n\nvoid change(struct X * p)\n{\n     free(p->name);       \n}\n\nint main()\n{\n    struct X x = {0};\n    x.name = strdup(\"a\");\n    change(&x);\n    printf(\"%s\", x.name);\n    /*forgot to free x.name and x.surname*/\n}\n
`;

sample["find-the-bug"]["bug-5"] =
`
#pragma safety enable\n\nvoid* _Owner _Opt calloc(unsigned int n, unsigned long size);\nvoid free(void* _Owner _Opt ptr);\n\nstruct Y {\n    int i; \n};\n\nstruct X {\n    int i; \n    struct Y* _Opt pY;\n};\n\nint main() \n{\n    struct X* _Owner _Opt pX = calloc(1, sizeof *pX);\n    if (pX) \n    {\n        struct Y* _Owner _Opt pY = calloc(1, sizeof *pY);\n        if (pY) \n        {\n            pX->pY = pY;\n            struct X* _Opt p = pX;\n            free(pY);            \n            p->pY->i = 1;  // no warning            \n        }\n        free(pX);\n    }\n}\n
`;

sample["find-the-bug"]["bug-6"] =
`
#pragma safety enable\n\n#include <stdlib.h>\n\nstruct Obj2 {\n    int i;\n};\n\nstruct Obj {\n    int i;\n    struct Obj2* _Opt _Owner obj2;\n};\n\nvoid main() \n{\n    struct Obj* _Opt _Owner o = calloc(1, sizeof *o);\n    if (o) {\n        o->obj2 = calloc(1, sizeof *o->obj2);\n        if (o->obj2) {\n            struct Obj* _Opt p = o;\n            free(o->obj2);\n            o->obj2 = nullptr;\n            p->obj2->i = 1; //warning: object is possibly null \n        }\n        free(o);\n    }\n}\n
`;

sample["find-the-bug"]["bug-7-k-r"] =
`
/*\n  This code is from \n  The C Programming Language 2 edition, page 145\n\n  There are two bugs int this original sample\n  - one memory leak\n  - one invalid state\n*/\n\n#pragma safety enable\n\n#include <stdlib.h>\n#include <string.h>\n\nstruct nlist {          /* table entry: */\n    struct nlist *next; /* next entry in chain */\n    char *name;  /* defined name */\n    char *defn;  /* replacement text */\n};\n\nstruct nlist *lookup(char *s);\n\n/* hash:  form hash value for string s */\nunsigned hash(char *s);\n\n\n#define HASHSIZE 101\n\nstatic struct nlist *hashtab[HASHSIZE]; /* pointer table */\n\n/*1* lookup: look for s in hashtab */\nstruct nlist *lookup(char *);\n\n\n/* install:  put (name, defn) in hashtab */\nstruct nlist *install(char *name, char *defn)\n{\n    struct nlist *np;\n    unsigned hashval;\n\n    if ((np = lookup(name)) == NULL) {  /* not found */\n        np = (struct nlist *) malloc(sizeof(*np));\n        if (np == NULL || (np->name = strdup(name)) == NULL)\n            return NULL;\n        hashval = hash(name);\n        np->next = hashtab[hashval];\n        hashtab[hashval] = np;\n    } else      /* already there */\n        free((void *) np->defn);  /* free previous defn */\n\n    if ((np->defn = strdup(defn)) == NULL)\n        return NULL;\n    return np;\n}\n
`;

sample["find-the-bug"]["bug-8"] =
`
#pragma safety enable\n\nvoid  f(int i)\n{\n    int k = 1;\n    int * p = &k;\n       \n    if (i) {\n       *p =0;\n    }\n    int h = 212/k;\n}\n\nint main() {}\n
`;

sample["find-the-bug"]["bug-9"] =
`
enum E1 {A, B};\nenum E2 {C, D};\n\nvoid f(enum E1 e)\n{\n    switch(e)\n    {\n        case A:break;\n        case D:break;\n    }\n}\nint main(){}\n
`;

sample["static-analysis"] = [];

sample["static-analysis"]["assert-state-static-debug"] =
`
#pragma safety enable\n\nvoid* _Owner _Opt _Uninitialized malloc(unsigned long size);\nvoid free(void* _Owner ptr);\n\nint main() {\n   void * _Owner  _Opt p = malloc(1);\n   if (p)\n   {\n     free(p);\n   }\n   static_debug(p);\n}\n
`;

sample["static-analysis"]["checking-double-free"] =
`
#pragma safety enable\n\nvoid free(void * _Owner p);\n\nstruct X {\n    char * _Owner naasdasdme;\n    char * _Owner nasdasame;\n    char * _Owner naasdme;\n    char * _Owner namasde;\n    char * _Owner namasade;\n};\n\nvoid x_destroy(_Dtor struct X * p)\n{\n    free(p->naasdasdme);\n    free(p->nasdasame);\n    free(p->naasdme);\n    free(p->namasde);\n    free(p->namasade);\n    free(p->namasde);\n}\n
`;

sample["static-analysis"]["ctor-and-dtor"] =
`
#pragma safety enable\n\n#include <stdlib.h>\n#include <string.h>\n\nstruct X {\n    char * _Owner _Opt text;\n};\n\nint init(_Out struct X *p)\n{\n    //comment and see what happens\n    p->text = strdup(\"a\");\n}\n\nvoid destructor(_Dtor struct X *p){\n    //comment and see what happens\n    free(p->text);\n}\n\nint main() {\n    struct X x;\n    init(&x);\n    destructor(&x);\n}\n
`;

sample["static-analysis"]["dynamic-array"] =
`
#pragma safety enable\n\n#include <stdlib.h>\n#include <assert.h>\n#include <errno.h>\n#include <stdio.h>\n#include <limits.h>\n#include <string.h>\n\nstruct int_array {\n    int* _Owner _Opt data;\n    int size;\n    int capacity;\n};\n\nint int_array_reserve(struct int_array* p, int n)\n{\n    if (n > p->capacity) {\n        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0])))) {\n            return EOVERFLOW;\n        }\n\n        void* _Owner _Opt pnew = realloc(p->data, n * sizeof(p->data[0]));\n        if (pnew == NULL) return ENOMEM;\n        override_state(p->data, \"moved\");\n        p->data = pnew;\n        p->capacity = n;\n    }\n    return 0;\n}\n\nint int_array_push_back(struct int_array* p, int value)\n{\n    if (p->size == INT_MAX) {\n        return EOVERFLOW;\n    }\n\n    if (p->size + 1 > p->capacity) {\n        int new_capacity = 0;\n        if (p->capacity > (INT_MAX - p->capacity / 2))\n        {\n            /*overflow*/\n            new_capacity = INT_MAX;\n        }\n        else {\n            new_capacity = p->capacity + p->capacity / 2;\n            if (new_capacity < p->size + 1) {\n                new_capacity = p->size + 1;\n            }\n        }\n\n        int error = int_array_reserve(p, new_capacity);\n        if (error != 0) {\n            return error;\n        }\n    }\n\n    p->data[p->size] = value;\n    p->size++;\n\n    return 0;\n}\n\nvoid int_array_destroy(_Dtor struct int_array* p)\n{\n    free(p->data);\n}\n\nint main()\n{\n    struct int_array a = { 0 };\n    int_array_push_back(&a, 1);\n    int_array_push_back(&a, 2);\n    int_array_destroy(&a);\n}\n
`;

sample["static-analysis"]["fix-me-1"] =
`
#pragma safety disable\n\n#include <stdlib.h>\n#include <string.h>\n\nstruct X {\n    char * text;\n};\n\nint main() {\n    struct X x = {};\n    x.text = strdup(\"a\");\n}\n
`;

sample["static-analysis"]["gives-ownership"] =
`
#pragma safety enable\n\n#include <string.h>\n#include <stdlib.h>\n\nconst char * _Owner _Opt gives_ownership() {\n    _Owner _Opt auto some_string = strdup(\"yours\");\n    return some_string;\n}\n\nint main(){\n    _Owner _Opt auto s = gives_ownership();\n    free(s);\n}\n
`;

sample["static-analysis"]["implementing-a-destructor-i"] =
`
#pragma safety enable\n\n#include <stdlib.h>\n#include <string.h>\n\nstruct X {\n    char *_Owner _Opt name;\n};\n\nvoid x_destroy(struct X x)\n{\n    free(x.name);\n}\n\nint main() {\n    struct X x = {0};\n    x.name = strdup(\"a\");\n    x_destroy(x);\n}\n
`;

sample["static-analysis"]["implementing-a-destructor-ii"] =
`
#pragma safety enable\n\nchar * _Opt _Owner strdup(const char* s);\nvoid free(void * _Opt _Owner p);\n\nstruct X {\n    char *_Owner _Opt name;\n};\n\nvoid x_destroy(_Dtor struct X * x)\n{\n    free(x->name);\n}\n\nint main() {\n    struct X x = {0};\n    x.name = strdup(\"a\");\n    x_destroy(&x);\n}\n
`;

sample["static-analysis"]["implementing-delete"] =
`
#pragma safety enable \n\nvoid * _Opt _Owner _Clear calloc(unsigned long n, unsigned long s);\nchar * _Opt _Owner strdup(const char* s);\nvoid free(void * _Opt _Owner p);\n\nstruct X {\n    char * _Owner _Opt text;\n};\n\nvoid x_delete(struct X * _Owner _Opt p)\n{\n    if (p)\n    {\n        free(p->text);\n        free(p);\n    }\n}\n\nint main() {\n    struct X * _Owner _Opt p = calloc(1, sizeof * p);\n    if (p){\n        p->text = strdup(\"a\");\n    }\n    x_delete(p);\n\n}\n
`;

sample["static-analysis"]["linked-list"] =
`
#pragma safety enable\n\n#include <stdlib.h>\n#include <assert.h>\n#include <string.h>\n\nstruct item {\n    char* _Owner title;\n    struct item* _Owner _Opt next;\n};\n\nstruct list {\n    struct item* _Owner _Opt head;\n    struct item* _Opt tail;\n};\n\nvoid list_insert_after(struct list* list,\n    struct item* item,\n    struct item* _Owner p_new_item)\n{\n    if (item->next == NULL) {\n        list->tail = p_new_item;\n    }\n    else {\n        assert(p_new_item->next == NULL);\n        p_new_item->next = item->next;\n    }\n\n    item->next = p_new_item;\n}\n\nvoid list_push_back(struct list* list,\n    struct item* _Owner p_item)\n{\n    if (list->head == NULL) {\n        list->head = p_item;\n    }\n    else {\n        assert(list->tail);\n        assert(list->tail->next == nullptr);\n        list->tail->next = p_item;\n    }\n    list->tail = p_item;\n}\n\nvoid list_push_front(struct list* list,\n    struct item* _Owner p_item)\n{\n    if (list->head == NULL) {\n        list->tail = p_item;\n    }\n    else {\n        assert(p_item->next == NULL);\n        p_item->next = list->head;\n    }\n    list->head = p_item;\n}\n\nvoid list_destroy(_Dtor struct list* list)\n{\n    struct item* _Owner _Opt it = list->head;\n    while (it != NULL) {\n        struct item* _Owner _Opt next = it->next;\n        free(it->title);\n        free(it);\n        it = next;\n    }\n}\n\nint main(int argc, char* argv[])\n{\n    struct list list = { 0 };\n    struct item* _Owner _Opt p_item = calloc(1, sizeof(struct item));\n    if (p_item)\n    {\n        char * _Owner _Opt title = strdup(\"title\");\n        if (title)\n        {\n            p_item->title = title;\n            list_push_front(&list, p_item);\n        }\n        else\n        {\n            free(p_item);\n        }\n    }\n    list_destroy(&list);\n}\n
`;

sample["static-analysis"]["malloc-free"] =
`
#pragma safety enable\n\nvoid * _Opt _Owner _Clear malloc(unsigned long s);\nvoid free(void * _Opt _Owner p);\n\nint main() {\n    void * _Owner _Opt p = malloc(1);\n    free(p);\n}\n
`;

sample["static-analysis"]["moving-parts-of-view"] =
`
#pragma safety enable\n\nvoid * _Opt _Owner _Clear calloc(unsigned long n, unsigned long s);\nchar * _Opt _Owner strdup(const char* s);\nvoid free(void * _Opt _Owner p);\n\nstruct X {\n    char * _Owner name;\n};\n\nstruct Y {\n    struct X x;\n    struct X * px;\n};\n\nvoid x_destroy(_Dtor struct X * p)\n{\n    free(p->name);\n}\n\nvoid f(struct Y * p)\n{\n    x_destroy(&p->x);\n    x_destroy(p->px);\n}\n\nint main() {\n    struct Y y = {};\n    y.x.name = strdup(\"a\");\n    f(&y);\n    free(y.x.name);\n}\n
`;

sample["static-analysis"]["mtx-t"] =
`
#pragma safety enable\n\nenum {\n    mtx_plain ,\n    mtx_timed,\n    mtx_plain,\n    mtx_timed,\n};\n\nenum {\n\n    thrd_success , /* unspecified */\n    thrd_nomem , /* unspecified */\n    thrd_timedout , /* unspecified */\n    thrd_busy , /* unspecified */\n    thrd_error /* unspecified */\n};\n\ntypedef struct { _Owner int dummy; } mtx_t;\nint mtx_init(_Out mtx_t *mtx, int type);\nvoid mtx_destroy( _Dtor mtx_t * mutex );\n\nint main()\n{\n    mtx_t mtx;\n    if (mtx_init(&mtx, mtx_plain) != thrd_success)\n    {\n        return 1;\n    }\n    mtx_destroy(&mtx);\n}\n
`;

sample["static-analysis"]["override-state-realloc"] =
`
#pragma safety enable\n\nvoid* _Owner _Opt realloc(void* _Opt ptr, unsigned size);\nvoid* _Owner _Opt _Uninitialized malloc(unsigned long size);\nvoid free(void* _Owner _Opt ptr);\n\nvoid f()\n{\n    void * _Owner _Opt p = malloc(1);\n    void * _Owner _Opt p2 = realloc(p, 2);\n    if (p2 != 0)\n    {\n        p = p2; //lint26 (semantic of realloc)\n    }\n    free(p);\n}\n
`;

sample["static-analysis"]["owner-assignment"] =
`
#pragma safety enable\n\nchar * _Opt _Owner strdup(const char* s);\nvoid free(void * _Opt _Owner p);\n\nint main()\n{\n    const char * _Owner _Opt s1 = strdup(\"hi\");\n    const char * _Owner _Opt s2 = nullptr;\n\n    s2 = s1; /*moved*/\n\n    free((void* _Owner)s2);\n}\n
`;

sample["static-analysis"]["owner-pointer-owns-two-objects"] =
`
#pragma safety enable\n\nvoid * _Owner _Opt calloc(unsigned long i, unsigned long sz);\nchar * _Owner _Opt strdup(const char* );\nvoid free(void * _Owner _Opt p);\n\nstruct X {\n    char *_Owner _Opt name;\n};\n\nint main()\n{\n    struct X * _Owner _Opt p = calloc(1, sizeof * p);\n    if (p) {\n\n        p->name = strdup(\"hi\");\n        struct X x = {0};\n        x = *p;\n        free(x.name);\n\n        free(p);\n    }\n}\n
`;

sample["static-analysis"]["socket"] =
`
_Owner int socket();\nvoid close(_Owner int fd);\n\nint main()\n{\n    _Owner int fd;\n\n    fd = socket();\n    if (fd < 0)\n    {\n        return 1;\n    }\n    close(fd);\n}\n
`;

sample["static-analysis"]["takes-ownership"] =
`
#pragma safety enable\n\nvoid * _Opt _Owner _Clear calloc(unsigned long n, unsigned long s);\nchar * _Opt _Owner strdup(const char* s);\nvoid free(void * _Opt _Owner p);\nvoid printf(char char* fmt, ...);\n\nvoid takes_ownership(char * _Owner _Opt some_string)\n{\n    if (some_string)\n    {\n        printf(\"%s\", some_string);\n        free(some_string);\n    }\n}\n\nint main()\n{\n    _Opt _Owner auto s = strdup(\"hello\");\n    takes_ownership(s /*moved here*/ );\n}\n
`;

sample["static-analysis"]["using-moved-object"] =
`
#pragma safety enable\n\nvoid * _Opt _Owner _Clear calloc(unsigned long n, unsigned long s);\nchar * _Opt _Owner strdup(const char* s);\nvoid free(void * _Opt _Owner p);\nvoid printf(char char* fmt, ...);\n\nstruct user\n{\n    int id;\n    char* _Owner _Opt name;\n};\n\nvoid set_id(struct user* p, int id){}\n\nint main()\n{\n    _Opt struct user user = {};\n    user.name = strdup(\"a\");\n    char* _Owner _Opt name = user.name;\n    free(name);\n    set_id(&user, 1); //warning: object 'user.name' was moved  \n}\n
`;

sample["static-analysis"]["view-qualifier"] =
`
#pragma safety enable \n\n#include <stdlib.h>\n#include <stdio.h>\n#include <string.h>\n\nstruct X {\n    char *_Owner _Opt name;\n};\n\nvoid f(_View struct X x)\n{\n    if (x.name)\n        printf(x.name);\n}\n\nint main() {\n    struct X x = {0};\n    x.name = strdup(\"a\");\n    f(x); /*not moved*/\n    free(x.name);\n}\n
`;

sample["static-analysis"]["x86_msvc\\assert-state-static-debug"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:59:16 */\n\nvoid *malloc(unsigned long size);\nvoid free(void * ptr);\n\nint main()\n{\n    void * p;\n\n    p = malloc(1);\n    if (p)\n    {\n        free(p);\n    }\n}\n
`;

sample["static-analysis"]["x86_msvc\\checking-double-free"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 10:59:40 */\n\nstruct X {\n    char * naasdasdme;\n    char * nasdasame;\n    char * naasdme;\n    char * namasde;\n    char * namasade;\n};\n\nvoid free(void * p);\n\nvoid x_destroy(struct X * p)\n{\n    free(p->naasdasdme);\n    free(p->nasdasame);\n    free(p->naasdme);\n    free(p->namasde);\n    free(p->namasade);\n    free(p->namasde);\n}\n
`;

sample["static-analysis"]["x86_msvc\\ctor-and-dtor"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:00:24 */\n\nstruct X {\n    char * text;\n};\n\nchar *__cdecl strdup(char * _String);\n\nint init(struct X * p)\n{\n    p->text = strdup(\"a\");\n}\n\nvoid __cdecl free(void * _Block);\n\nvoid destructor(struct X * p)\n{\n    free(p->text);\n}\n\nint main()\n{\n    struct X x;\n\n    init(&x);\n    destructor(&x);\n}\n
`;

sample["static-analysis"]["x86_msvc\\dynamic-array"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:01:50 */\n\nstruct int_array {\n    int * data;\n    int size;\n    int capacity;\n};\n\nstatic void * memset2(void *dest, int ch, unsigned int count);\n\nvoid *__cdecl realloc(void * _Block, unsigned int _Size);\n\nint int_array_reserve(struct int_array * p, int n)\n{\n    if (n > p->capacity)\n    {\n        void * pnew;\n\n        if ((unsigned int)n > 1073741823)\n        {\n            return 132;\n        }\n        pnew = realloc(p->data, n * 4);\n        if (pnew == 0)\n        {\n            return 12;\n        }\n        p->data = pnew;\n        p->capacity = n;\n    }\n    return 0;\n}\n\nint int_array_push_back(struct int_array * p, int value)\n{\n    if (p->size == 2147483647)\n    {\n        return 132;\n    }\n    if (p->size + 1 > p->capacity)\n    {\n        int new_capacity;\n        int error;\n\n        new_capacity = 0;\n        if (p->capacity > (2147483647 - p->capacity / 2))\n        {\n            new_capacity = 2147483647;\n        }\n        else\n        {\n            new_capacity = p->capacity + p->capacity / 2;\n            if (new_capacity < p->size + 1)\n            {\n                new_capacity = p->size + 1;\n            }\n        }\n        error = int_array_reserve(p, new_capacity);\n        if (error != 0)\n        {\n            return error;\n        }\n    }\n    p->data[p->size] = value;\n    p->size++;\n    return 0;\n}\n\nvoid __cdecl free(void * _Block);\n\nvoid int_array_destroy(struct int_array * p)\n{\n    free(p->data);\n}\n\nint main()\n{\n    struct int_array a;\n\n    memset2(&a, 0, 12);\n    int_array_push_back(&a, 1);\n    int_array_push_back(&a, 2);\n    int_array_destroy(&a);\n}\n\nstatic void * memset2(void *ptr, int value, unsigned int count)\n{\n    unsigned char *p;\n    unsigned char v;\n\n    p = (unsigned char *) ptr;\n    v = (unsigned char) value;\n    while (count--)\n    {\n        *p++ = v;\n    }\n\n    return ptr;\n}\n
`;

sample["static-analysis"]["x86_msvc\\fix-me-1"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:01:52 */\n\nstruct X {\n    char * text;\n};\n\nstatic void * memset2(void *dest, int ch, unsigned int count);\n\nchar *__cdecl strdup(char * _String);\n\nint main()\n{\n    struct X x;\n\n    memset2(&x, 0, 4);\n    x.text = strdup(\"a\");\n}\n\nstatic void * memset2(void *ptr, int value, unsigned int count)\n{\n    unsigned char *p;\n    unsigned char v;\n\n    p = (unsigned char *) ptr;\n    v = (unsigned char) value;\n    while (count--)\n    {\n        *p++ = v;\n    }\n\n    return ptr;\n}\n
`;

sample["static-analysis"]["x86_msvc\\implementing-a-destructor-ii"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:02:48 */\n\nstruct X {\n    char * name;\n};\n\nstatic void * memset(void *dest, int ch, unsigned int count);\n\nvoid free(void * p);\n\nvoid x_destroy(struct X * x)\n{\n    free(x->name);\n}\n\nchar *strdup(char * s);\n\nint main()\n{\n    struct X x;\n\n    memset(&x, 0, 4);\n    x.name = strdup(\"a\");\n    x_destroy(&x);\n}\n\nstatic void * memset(void *ptr, int value, unsigned int count)\n{\n    unsigned char *p;\n    unsigned char v;\n\n    p = (unsigned char *) ptr;\n    v = (unsigned char) value;\n    while (count--)\n    {\n        *p++ = v;\n    }\n\n    return ptr;\n}\n
`;

sample["static-analysis"]["x86_msvc\\implementing-delete"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:04:09 */\n\nstruct X {\n    char * text;\n};\n\nvoid free(void * p);\n\nvoid x_delete(struct X * p)\n{\n    if (p)\n    {\n        free(p->text);\n        free(p);\n    }\n}\n\nvoid *calloc(unsigned long n, unsigned long s);\nchar *strdup(char * s);\n\nint main()\n{\n    struct X * p;\n\n    p = calloc(1, 4);\n    if (p)\n    {\n        p->text = strdup(\"a\");\n    }\n    x_delete(p);\n}\n
`;

sample["static-analysis"]["x86_msvc\\malloc-free"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:04:43 */\n\nvoid *malloc(unsigned long s);\nvoid free(void * p);\n\nint main()\n{\n    void * p;\n\n    p = malloc(1);\n    free(p);\n}\n
`;

sample["static-analysis"]["x86_msvc\\moving-parts-of-view"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:05:15 */\n\nstruct X {\n    char * name;\n};\n\nstruct Y {\n    struct X x;\n    struct X * px;\n};\n\nstatic void * memset(void *dest, int ch, unsigned int count);\n\nvoid free(void * p);\n\nvoid x_destroy(struct X * p)\n{\n    free(p->name);\n}\n\nvoid f(struct Y * p)\n{\n    x_destroy(&p->x);\n    x_destroy(p->px);\n}\n\nchar *strdup(char * s);\n\nint main()\n{\n    struct Y y;\n\n    memset(&y, 0, 8);\n    y.x.name = strdup(\"a\");\n    f(&y);\n    free(y.x.name);\n}\n\nstatic void * memset(void *ptr, int value, unsigned int count)\n{\n    unsigned char *p;\n    unsigned char v;\n\n    p = (unsigned char *) ptr;\n    v = (unsigned char) value;\n    while (count--)\n    {\n        *p++ = v;\n    }\n\n    return ptr;\n}\n
`;

sample["static-analysis"]["x86_msvc\\mtx-t"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:06:05 */\n\nstruct __tag2 {\n    int dummy;\n};\n\nint mtx_init(struct __tag2 * mtx, int type);\nvoid mtx_destroy(struct __tag2 * mutex);\n\nint main()\n{\n    struct __tag2 mtx;\n\n    if (mtx_init(&mtx, 2) != 0)\n    {\n        return 1;\n    }\n    mtx_destroy(&mtx);\n}\n
`;

sample["static-analysis"]["x86_msvc\\override-state-realloc"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:07:38 */\n\nvoid *malloc(unsigned long size);\nvoid *realloc(void * ptr, unsigned int size);\nvoid free(void * ptr);\n\nvoid f()\n{\n    void * p;\n    void * p2;\n\n    p = malloc(1);\n    p2 = realloc(p, 2);\n    if (p2 != 0)\n    {\n        p = p2;\n    }\n    free(p);\n}\n
`;

sample["static-analysis"]["x86_msvc\\owner-assignment"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:08:18 */\n\nchar *strdup(char * s);\nvoid free(void * p);\n\nint main()\n{\n    char * s1;\n    char * s2;\n\n    s1 = strdup(\"hi\");\n    s2 = 0;\n    s2 = s1;\n    free((void *)s2);\n}\n
`;

sample["static-analysis"]["x86_msvc\\owner-pointer-owns-two-objects"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:08:22 */\n\nstruct X {\n    char * name;\n};\n\nstatic void * memset(void *dest, int ch, unsigned int count);\n\nvoid *calloc(unsigned long i, unsigned long sz);\nchar *strdup(char *);\nvoid free(void * p);\n\nint main()\n{\n    struct X * p;\n\n    p = calloc(1, 4);\n    if (p)\n    {\n        struct X x;\n\n        p->name = strdup(\"hi\");\n        memset(&x, 0, 4);\n        x = *p;\n        free(x.name);\n        free(p);\n    }\n}\n\nstatic void * memset(void *ptr, int value, unsigned int count)\n{\n    unsigned char *p;\n    unsigned char v;\n\n    p = (unsigned char *) ptr;\n    v = (unsigned char) value;\n    while (count--)\n    {\n        *p++ = v;\n    }\n\n    return ptr;\n}\n
`;

sample["static-analysis"]["x86_msvc\\socket"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:08:35 */\n\nint socket();\nvoid close(int fd);\n\nint main()\n{\n    int fd;\n\n    fd = socket();\n    if (fd < 0)\n    {\n        return 1;\n    }\n    close(fd);\n}\n
`;

sample["static-analysis"]["x86_msvc\\takes-ownership"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:09:04 */\n\nvoid printf(char * fmt, ...);\nvoid free(void * p);\n\nvoid takes_ownership(char * some_string)\n{\n    if (some_string)\n    {\n        printf(\"%s\", some_string);\n        free(some_string);\n    }\n}\n\nchar *strdup(char * s);\n\nint main()\n{\n    char * s;\n\n    s = strdup(\"hello\");\n    takes_ownership(s);\n}\n
`;

sample["static-analysis"]["x86_msvc\\using-moved-object"] =
`
/* Cake 0.14.18 x86_msvc 2026-08-07 11:09:52 */\n\nstruct user {\n    int id;\n    char * name;\n};\n\nstatic void * memset(void *dest, int ch, unsigned int count);\n\nvoid set_id(struct user * p, int id)\n{\n}\n\nchar *strdup(char * s);\nvoid free(void * p);\n\nint main()\n{\n    struct user user;\n    char * name;\n\n    memset(&user, 0, 8);\n    user.name = strdup(\"a\");\n    name = user.name;\n    free(name);\n    set_id(&user, 1);\n}\n\nstatic void * memset(void *ptr, int value, unsigned int count)\n{\n    unsigned char *p;\n    unsigned char v;\n\n    p = (unsigned char *) ptr;\n    v = (unsigned char) value;\n    while (count--)\n    {\n        *p++ = v;\n    }\n\n    return ptr;\n}\n
`;

