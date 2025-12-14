var sample = {};

sample["C89"] = [];
sample["C89"]["enuns"] =
`
enum escapes { BELL = '\\a', BACKSPACE = '\\b', TAB = '\\t',
               NEWLINE = '\\n', VTAB = '\\v', RETURN = '\\r' };

enum months { JAN = 1, FEB, MAR, APR, MAY, JUN,
              JUL, AUG, SEP, OCT, NOV, DEC };
              /* FEB is 2, MAR is 3, etc. */
int main()
{
    enum months m  = FEB;
}
`;


sample["C99"] = [];
sample["C99"]["_Bool"] =
    `

int main(void)
{
    _Bool b0 = false;
    _Bool b1 = true;
    _Bool b2 = 123;

    b1 = b0;
    b2 = 1234;
    return 0;
}

void f1(_Bool b, int i){}

_Bool f2(int i, int j)
{
    _Bool b = i;

    f1(i, i);
    f1(i = j, i = j);
    f1(1 ? i : j, 1 ? i : j);

    b = i;
    b = i = j;
    b = i = j && i;
    b = 1 ? i : j;
    b = 1, i;
    return i;
}

`;

sample["C99"]["Flexible array members"] =
    `

#include <stdio.h>
#include <stdlib.h>

struct X {
    int count;
    double values[]; // flexible array
};

/*
    The size of a structure with a flexible array member is
    determined as if the flexible array member were omitted,
    EXCEPT that it may have more trailing padding than the
    omission would imply
*/

int main() {

    int n = 3;

    printf("sizeof(struct X) = %d\\n", (int) sizeof(struct X));
    printf("allocated = %d\\n", (int) sizeof(struct X) + n * sizeof(double));

    struct X* p = malloc(sizeof(struct X) + n * sizeof(double));
    if (p == NULL) return 0;

    p->count = n;
    p->values[0] = 10.0;
    p->values[1] = 20.0;
    p->values[2] = 30.0;

    for (int i = 0; i < p->count; ++i)
        printf("%f\\n", p->values[i]);

    free(p);

    return 0;
}
`;

sample["C99"]["Designated Initializers"] =
    `


struct T {
    int k;
    int l;
};

struct S {
    int i;
    struct T t;
};

struct T x = {
    .l = 43,
    .k = 42,
};

void f(void) {
    struct S l = {
        1,
        .t = x,
        .t.l = 41,
    };
}

int main() {
    int a[6] = {[4] = 29, [2] = 15};

    struct point {
        int x, y;
    };
    struct point p = {.y = 2, .x = 3};

    struct {
        int a[3], b;
    } w[] = {[0].a = {1}, [1].a[0] = 2};
}

#define A_MAX 20
int a[A_MAX] = { 1, 3, 5, 7, 9, [A_MAX-5] = 8, 6, 4, 2, 0 };

`;

sample["C99"]["Compound literals"] =
    `
struct X {
  int i;
};

int main(){
   struct X x = {};
   x = (struct X){};
}
`;


sample["C99"][" Hexadecimal floating constants"] =
    `
const double d = 0x1p+1;
const double dmax = 0x1.fffffffffffffp+1023;
const double dmin = 0x1p-1022;

/*
  Note : The result may not be so precise as the original
*/
`;

sample["C99"]["_Pragma"] =
    `
//6.10.11 Pragma operator

#define LISTING(x) PRAGMA(listing on #x)
#define PRAGMA(x) _Pragma(#x)

LISTING (..\listing.dir)

`;

sample["C99"]["__func__"] =
    `
#include <stdio.h>
int main()
{
    printf("%s\\n", __func__);
    printf("%s\\n", __func__);
}
`;


sample["C99"]["init-clause of the for loop"] =
    `
int main()
{
   const int max = 10;
   for (int n = max - 1; n >= 0; n--)
   {
     // body of loop
   }
}
`;

sample["C99"]["inline"] =
    `
inline int sum(int a, int b)
{
    return a + b;
}

int main()
{
    int r = sum(1,3);
}
`;

sample["C99"]["restrict pointers"] =
    `
   void copy_array(int n, int *restrict a, int *restrict b)
   {
      while (n-- > 0)
        *a++ = *b++;
   }
`;

sample["C99"]["Complex"] =
    `
   /*not implemented*/
`;



sample["C99"]["int a[static]"] =
    `
#include <stdlib.h>


void F(int a[static const 5]) 
{
    //cake was a warning when modifying array arguments
    a = 0;

    //bounds check for constant indexes
    int k = a[5];
}


int main() {
    
    F(0);
    F(NULL);
    F(nullptr);

    int a[] = {1, 2, 3};    
    //F(a); /*try*/
    
    int b[] = { 1, 2, 3 , 4, 5};
    F(b);
    int k = b[5]; //bounds check

    int c[] = { 1, 2, 3 , 4, 5, 6};
    F(c);
}

`;

sample["C99"][" __VA_ARGS__"] =
    `
#include <stdio.h>

#define MYLOG(FormatLiteral, ...)  fprintf (stderr, "%s(%u): " FormatLiteral "\\n", __FILE__, __LINE__, __VA_ARGS__)

#pragma expand MYLOG

int main()
{
 MYLOG("Too many balloons %u", 42);
}


`;

sample["C11"] = [];

sample["C11"]["anonymous structures/unions"] =
    `
struct point { float x, y, z; };

struct location {
    char* name;
    struct point;
};

int main()
{
    struct location location = {};
    location.x = 1;
    location.y = 2;
}
`;

sample["C11"]["_Generic"] =
    `
#include <math.h>

#define cbrt(X) _Generic((X), \\
                  double: cbrtl, \\
                  float: cbrtf ,\\
                  default: cbrtl  \\
              )(X)

int main(void)
{
    cbrt(1.0);

    const int * const p = 0;
    _Static_assert(_Generic(p, const int * : 1 ), "");
    _Static_assert(_Generic(&p, const int * const * : 1 ), "");
    _Static_assert(_Generic(main, int (*)(void) : 1 ), "");

    const int * const p2 = 0;
    static_assert(_Generic(p2, const int *: 1));

    static_assert(_Generic("abc",  char *: 1));    
}

`;

sample["C11"]["_Static_assert"] =
    `
/*
   See also C23 static_assert
*/

int main()
{
    _Static_assert(1 == 1, "error");    
}
`;


sample["C11"]["_Noreturn"] =
    `
#include <stdio.h>
#include <stdlib.h>

_Noreturn void not_coming_back(void)
{
    puts("There's no coming back");
    exit(1);
    return;
}
`;

sample["C11"]["u8 literals"] =
    `
/*
* cake input source code encode is always utf8
* cake output source code is also utf8

* u8 prefix may be useful in case you have a compiler where
* the input or output is not uft8.
*/

#include <stdio.h>

int main()
{
  printf("Hello, 世界\\n");
  printf(u8"Hello, 世界\\n");
}
`;

sample["C11"]["u U char literals"] =
    `
int main()
{
  int i = U'ç';
  int i2 = u'ç';
}
`;

sample["C11"]["_Alignof / alignof (in C23)"] =
    `
struct X
{
    char s;
    //align member 7
    double c;
    int s2;
    //align member 4
};

int main(void)
{
    static_assert(_Alignof(float[10]) == alignof(float));
    static_assert(alignof(char) == 1);
    static_assert(sizeof(struct X) == 24);
    static_assert(alignof(struct X) == 8);
}

`;

sample["C11"]["_Alignas / alignas (in C23)"] =
    `
#include <stdio.h>

// every object of type struct sse_t will be aligned to 16-byte boundary
// (note: needs support for DR 444)
struct sse_t
{
    alignas(16) float sse_data[4];
};

// every object of type struct data will be aligned to 128-byte boundary
struct data
{
    char x;
    alignas(128) char cacheline[128]; // over-aligned array of char,
                                      // not array of over-aligned chars
};

int main(void)
{
    printf("sizeof(data) = %zu (1 byte + 127 bytes padding + 128-byte array)\\n",
           sizeof(struct data));

    printf("alignment of sse_t is %zu\\n", alignof(struct sse_t));

    alignas(2048) struct data d; // this instance of data is aligned even stricter
    (void)d; // suppresses "maybe unused" warning
}

/*
   OUTPUT
   sizeof(data) = 256 (1 byte + 127 bytes padding + 128-byte array)
   alignment of sse_t is 16
*/

`;

sample["C11"]["_Thread_local / thread_local (in C23)"] =
    `
thread_local int a;

int main(void)
{    
}

`;
//

sample["C23"] = []
sample["C23"]["Digit Separator"] =
    `
int main()
{
    int a = 1000'00;
    static_assert(1000'00 == 100000);
}
`;

sample["C23"]["Binary Literal"] =
    `
int main()
{
    int b = 0B10;
}
`;


sample["C23"]["static_assert"] =
    `
//https://en.cppreference.com/w/c/language/_Static_assert
#include <assert.h> // no longer needed since C23

int main(void)
{
    // Test if math works, C23:
    static_assert((2 + 2) % 3 == 1, "Whoa dude, you knew!");
    // Pre-C23 alternative:
    _Static_assert(2 + 2 * 2 == 6, "Lucky guess!?");

    // This will produce an error at compile time.
    // static_assert(sizeof(int) < sizeof(char), "Unmet condition!");

    constexpr int _42 = 2 * 3 * 2 * 3 + 2 * 3;
    static_assert(_42 == 42); // the message string can be omitted.

    // const int _13 = 13;
    // Compile time error - not an integer constant expression:
    // static_assert(_13 == 13);
}
`;


sample["C23"]["#elifdef  #elifndef"] =
    `
/*
  C23 preprocessing directives elifdef and elifndef N2645
  https://open-std.org/jtc1/sc22/wg14/www/docs/n2645.pdf
*/

#define Y

#ifdef X
#define VERSION 1
#elifdef  Y
#define VERSION 2
#else
#define VERSION 3
#endif

_Static_assert(VERSION == 2, "");

`;


sample["C23"]["__VA_OPT__"] =
    `
/*
  __VA_OPT__ lets you optionally insert tokens depending on
  if a variadic macro is invoked with additional arguments. 
  
  (Select: Compile To Preprocess only)
*/

#define M(X, ...) X __VA_OPT__(,) __VA_ARGS__
M(1)    // expands to 1
M(1, 2) // expands to 1, 2

#define F(...) f(0 __VA_OPT__(,) __VA_ARGS__)
#define G(X, ...) f(0, X __VA_OPT__(,) __VA_ARGS__)
#define SDEF(sname, ...) S sname __VA_OPT__(= { __VA_ARGS__ })
#define EMP
F(a, b, c)           // replaced by f(0, a, b, c)
F()                  // replaced by f(0)
F(EMP)               // replaced by f(0)
G(a, b, c)           // replaced by f(0, a, b, c)
G(a, )               // replaced by f(0, a)
G(a)                 // replaced by f(0, a)
SDEF(foo);           // replaced by S foo;
SDEF(bar, 1, 2);     // replaced by S bar = { 1, 2 };


#define H2(X, Y, ...) __VA_OPT__(X ## Y,) __VA_ARGS__
H2(a, b, c, d)       // replaced by ab, c, d

#define H3(X, ...) #__VA_OPT__(X##X X##X)
H3(, 0)              // replaced by ""
  
#define H4(X, ...) __VA_OPT__(a X ## X) ## b
H4(, 1)              // replaced by a b

#define H5A(...) __VA_OPT__()/**/__VA_OPT__()
#define H5B(X) a ## X ## b
#define H5C(X) H5B(X)
H5C(H5A())          // replaced by ab



`;

sample["C23"]["_has_include|__has_embed|__has_c_attribute"] =
    `

#if __has_include(<stdio.h>)
#warning  yes we have <stdio.h>
#endif


#if __has_embed(<stdio.h>)
#warning  yes we have <stdio.h> embed
#endif


#if __has_include(<otherfile.h>)
#warning  YES
#else
#warning  NO we dont have <otherfile.h>
#endif


#if __has_c_attribute(fallthrough)
#else
#warning at this moment we return 0 for all attributes
#endif

/*
  __has_include is a sample of feature that is impossible to translate, 
  unless for immediate compilation.
*/
`;

sample["C23"]["#embed"] =
    `
#include <stdio.h>

int main()
{
  static const char file_txt[] = {
   #embed "stdio.h"
   ,0
  };

  printf("%s\\n", file_txt);

}

/* 
  Note
  The idea is to have a mode where cake generates a file to 
  be included like this:

  static const char file_txt[] = {
   #include "stdio.h.embed"
   ,0
  };

*/
`;

sample["C23"]["u8 char literal"] =
    `
//https://open-std.org/JTC1/SC22/WG14/www/docs/n2418.pdf
int main()
{
   unsigned char c = u8'~';
}
`;

sample["C23"]["#warning"] =
    `
#include <stdio.h>

int main()
{
  #warning TODO ..missing code  
}
`;

sample["C23"]["empty initializer"] =
    `
int main()
{
    struct X {
        int i;
    } x = {};

    x = (struct X) {};

    struct Y
    {
        struct X x;
    } y = { {} };
}

`;

sample["C23"]["typeof / typeof_unqual"] =
    `
#include <stdlib.h>

#define SWAP(a, b) \\
  do {\\
    typeof(a) temp = a; a = b; b = temp; \\
  } while (0)


/*pointer to function returning int*/
int (*g1)(int a);
typeof(g1) g2;

/*function returning int* */
int * f1(int a);

/*function returning int* */
typeof(f1) f2;

/*pointer to function returning int*/
typeof(f1) *f3;


typeof(1 + 1) f()
{
  return 1;
}

void f4(int a[2]) {
    typeof(a) p;
}

int main()
{
    /*simple case*/
    int a = 1;
    typeof(a) b = 1;

    /*pay attention to the pointer*/
    typeof(int*) p1, p2;
    typeof(typeof(int*)) p3;
    typeof(typeof(p1)) p4;
    

    typeof_unqual(const int) p5;
    typeof_unqual(const int * const) p6;
    
    

    /*let's expand this macro and see inside*/
    SWAP(a, b);

    /*for anonymous structs we insert a tag*/
    struct { int i; } x;
    typeof(x) x2;
    typeof(x) x3;

   /*Things get a little more complicated*/
   int *array[2];
   typeof(array) a1, a2;
   
   typeof(array) a3[3];
   typeof(array) *a4[4];

   /*abstract declarator*/
   int k = sizeof(typeof(array));


   /*new way to declare pointer to functions?*/
   typeof(void (int)) * pf = NULL;
}


int f5(){
  typeof(int [2]) *p1 = 0;
  auto p2 = (typeof(int [2]) *) p1 ;
}


`;


sample["C23"]["auto"] =
    `


/*function sample*/
extern int func(void);
auto p_func = func;
const auto pc_func = func;

/*using auto inside a macro*/
#define SWAP(a, b)   do {    auto temp = a; a = b; b = temp;   } while (0)

void f()
{
  int a = 1;
  int b = 2;
  SWAP(a, b);
}

auto literal_string = "hello auto";

struct {int i;} x;


auto bb = true;
auto pnull = nullptr;

/*arrays*/
int a5[5];
auto a = a5; /*lvalue*/

auto pa = &a5;

const auto cpa = &a5;

void f2(int a[2]){
  auto p = a;
}


int main()
{
  double const x = 78.9;
  double y = 78.9;
  auto q = x;
  auto const p = &x;
  auto const r = &y;
}


`;


sample["C23"]["bool true false"] =
    `

#include <stdio.h>

int main()
{
  bool b = true;
  b = false;
  static_assert(1 == true);
  static_assert(0 == false);

  printf("%s", _Generic(true, bool : "bool"));
  printf("%s", _Generic(false, bool : "bool"));

  printf("%s", _Generic(b, bool : "bool"));

  auto b2 = true;
  printf("%s", _Generic(b2, bool : "bool"));
}
`;

sample["C23"]["nullptr"] =
    `

#include <stdlib.h>
#include <stdio.h>

int main()
{
  void * p = nullptr;
  void * p2 = NULL;

  auto a = nullptr;
  
  printf("%s", _Generic(nullptr, typeof(nullptr) : "nullptr_t"));
}
`;


sample["C23"]["[[maybe_unused]] "] =
    `

void f( [[maybe_unused]] int arg1, int arg2)
{
    [[maybe_unused]] int local1;
    int local2;
    /*warning not used for local2*/
    /*warning not used for arg2*/
}

`;


sample["C23"]["[[deprecated]] "] =
    `
[[deprecated]] void f2() {
}


struct [[deprecated]] S {
  int a;
};

enum [[deprecated]] E1 {
 one
};

int main(void) {
    struct S s;
    enum E1 e;
    f2();
}
`;

sample["C23"]["[[nodiscard]] "] =
    `

#include <stdlib.h>

struct [[nodiscard]] error_info { int error; };

struct error_info enable_missile_safety_mode(void);

void launch_missiles(void);

void test_missiles(void) {
    enable_missile_safety_mode();
    launch_missiles();
}

[[nodiscard("must check armed state")]]
bool arm_detonator(int within);

void detonate();

void call(void) {
  arm_detonator(3);
  detonate();
}



`;

sample["C23"]["[[fallthrough]] "] =
    `
/*
   IS NOT IMPLEMENTED YET
*/

void f(int n) {
    void g(void), h(void), i(void);
    switch (n) {
    case 1: /* diagnostic on fallthrough discouraged */
    case 2:
        g();
        [[fallthrough]];
    case 3: /* diagnostic on fallthrough discouraged */
        do {
            [[fallthrough]]; /* constraint violation: next statement is not
            part of the same secondary block execution */
        } while (false);
    case 6:
        do {
            [[fallthrough]]; /* constraint violation: next statement is not
            part of the same secondary block execution */
        } while (n--);
    case 7:
        while (false) {
            [[fallthrough]]; /* constraint violation: next statement is not
            part of the same secondary block execution */
        }
    case 5:
        h();
    case 4: /* fallthrough diagnostic encouraged */
        i();
        [[fallthrough]]; /* constraint violation */
    }
}
`;



sample["C23"]["constexpr"] =
    `
#include <stdio.h>

constexpr int c = 123;

constexpr int c2 = c + 1000;

int a[c];

constexpr double PI = 3.14;


static_assert(PI + 1 == 3.14 + 1.0);

struct Y {
    int a;
    int ar[3];
    int b;
};

void T3()
{
    constexpr struct Y y = { .ar[1] = 2, 3, 4 };
    static_assert(y.a == 0);
    static_assert(y.ar[0] == 0);
    static_assert(y.ar[1] == 2);
    static_assert(y.ar[2] == 3);
    static_assert(y.b == 4);
    static_assert(y.ar[1] + y.ar[2] == 5);
}

static_assert("abc"[0] == 'a');


int main()
{
    constexpr char ch = 'a';

    printf("%f %c", PI, ch);
}

`;

sample["C23"]["Enhancements to Enumerations"] =
    `
//https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3030.htm
enum X : short {
    A
};

int main() {
    enum X x = A;
    static_assert(sizeof(x) == sizeof(short));
}
`;

sample["C23"]["Improved Enumerations"] =
    `
//TODO
//https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3029.htm

enum a {
	a0 = 0xFFFFFFFFFFFFFFFFULL
};

int main () {}
`;

sample["C23"]["_Bitint(N)"] =
    `
//TODO

`;


sample["C2Y"] = [];

sample["C2Y"]["_Generic(type-name)"] =
    `
/*
  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3214.pdf
*/

int main(void)
{
    const int * const p = nullptr;
    static_assert(_Generic(p, const int * : 1 ));
    static_assert(_Generic(typeof(p), const int * const: 1));
  
    static_assert(_Generic(int, int : 1));    
    static_assert(_Generic(typeof("abc"), char [4]: 1));
}

`;

sample["C2Y"]["new 0o and 0O prefixes for octal literals"] =
    `
 //Obsolete implicitly octal literals
 //https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3319.htm

static_assert(0o52 == 052);
static_assert(0O52 == 052);
static_assert(0O52 == 42);

int main()
{
    int i = 0o52;
}
`;

sample["C2Y"]["if declarations"] =
    `
/*
  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3356.htm
  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3580.htm
*/

#include <stdio.h>

int main()
{
   
   FILE* f0;
   if ( f0 = fopen("file.txt", "r"))
   {
     /*...*/
     fclose(f0);
   }
   
   if (FILE* f = fopen("file.txt", "r"); f)
   {
     /*...*/
     fclose(f);
   }

   if (FILE* f = fopen("file.txt", "r"))
   {    
     /*...*/
     fclose(f);
   }
}
`;

sample["C2Y"]["_Countof"] =
    `
/*
  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3369.pdf
*/

void f(int n)
{
  int v[123][n];
  static_assert(_Countof(v) == 123);
}

int main()
{
  int a[7][3];
  int n = _Countof(a);
  static_assert(_Countof(a) == 7);

  int n2 = _Countof(int [7][3]);
  static_assert(_Countof(int [2][3]) == 2);
}

`;


sample["C2Y"]["case range ..."] =
    `
  //Case range expressions
  //https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3370.htm

  #include <stdio.h>

  void f(int n)
  {
    switch (n)
    {
       case 1 ... 10:
       printf("n in range 1...10\\n");
       break;
       default:
       break;
    }
  }

  int main(){
    f(1);
    f(11);
  }
`;

sample["C2Y"]["#def"] =
    `
/*
  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3524.txt
*/

#def foo(x)
		do {
			bar(x);
			baz(x);
		}
        while (0)
#enddef

foo(1)
foo(2)

`;

sample["C2Y"]["#def II"] =
    `
  // Add directives #def and #enddef
  // https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3524.txt

/*
  use -preprocess-def-macro
  to preprocess #def macro after expansion
*/


#def foo(x)
		do {
#if 0
			bar(x);
#endif
			baz(x);
		}
        while (0)
#enddef

foo(2)

`;



sample["C2Y"]["_Defer inside try blocks"] =
    `
/*
  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3199.htm
*/

#include <stdio.h>

int main()
{

  try
  {
     FILE* f = fopen("in.txt", "r");
     if (f == NULL) throw;
     _Defer fclose(f);

     FILE* f2 = fopen("out.txt", "w");
     if (f2 == NULL) throw;
     _Defer fclose(f2);

     //...

    /*success here*/
  }
  catch
  {
     /*some error*/
  }


}

`;

sample["C2Y"]["_Defer with breaks III"] =
    `

#include <stdio.h>

int main()
{

  do
  {
     FILE* f = fopen("in.txt", "r");
     if (f == NULL) break;
     _Defer fclose(f);

     FILE* f2 = fopen("out.txt", "w");
     if (f2 == NULL) break;
     _Defer fclose(f2);

     //...

    /*success here*/
  }
  while(0);


}

`;


sample["C2Y"]["_Defer with breaks IV"] =
    `

#include <stdio.h>

int main()
{
  FILE* f = NULL;
  _Defer if (f) fclose(f);

  do
  {
     f = fopen("in.txt", "r");
     if (f == NULL) break;     
  }
  while(0);

}

`;


sample["C2Y"]["_Defer with return V"] =
    `

#include <stdio.h>

int main()
{
  FILE* f = fopen("in.txt", "r");
  if (f == NULL) return 1;
  _Defer fclose(f);

  FILE* f2 = fopen("out.txt", "w");
  if (f2 == NULL) return 1;
  _Defer fclose(f2);

  return 0;
}


`;


sample["C2Y"]["_Defer goto VI"] =
    `

#include <stdio.h>

int main()
{
  FILE* f = fopen("in.txt", "r");
  if (f != NULL)
  {
     _Defer fclose(f);

     FILE* f2 = fopen("out.txt", "w");
     if (f2 == NULL) goto LEND;
     _Defer fclose(f2);
  }
  LEND:
  return 0;
}

`;


sample["C2Y"]["_Defer and flow analysis"] =
    `
#pragma flow enable

void f2(int i){}
void f(int k)
{
  int i;
  _Defer f2(i);
  
  if (k > 1)
   return;
}

`;


sample["C2Y"]["_Defer interleaved with return"] =
    `

int f(){
  int i = 1;
  _Defer {
    i = 0;
  }
  return i++;
}

void f0(){
  int i = 1;
  _Defer {
    i = 0;
  }
  return; //empty or constant expression
}


`;

sample["C2Y"]["__COUNTER__"] =
    `
//https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3457.htm#number-of-expansions

#define X(Z) Z Z
X(__COUNTER__) // 0 0
X(__COUNTER__) // 1 1

`;



sample["C2Y"]["Local functions I"] =
    `
int main()
{
	static int dup(int a) { return a * 2; }
    return dup(1);
}
`;

sample["C2Y"]["Local functions II"] =
    `
#include <stdlib.h>

void async(void (* callback)(int result, void * data), void * data);

int main()
{
	struct {int value; }* capture = calloc(1, sizeof * capture);
    static void callback(int result, void * data)
    {
		typeof(capture) p = data;
        free(p);
    }

    async(callback, capture);
}
`;

sample["C2Y"]["Local functions III"] =
    `
#include <stdio.h>

void f() {
    printf("extern function\\n");
}

int main()
{    
    static void f(); //forward declaration

    static void f() {
        printf("local function\\n");
    }

    f(); //calls local function
}
`;



sample["C2Y"]["Literal function async I"] =
    `
#include <stdlib.h>
#include <stdio.h>

void async(void (* callback)(int result, void * data), void * data)
{
   callback(1, data);
}

int main()
{
	struct capture {int value; }* capture = calloc(1, sizeof * capture);
    if (capture == 0) return 1;

    capture->value = 123;
    async((void (int result, void * data))
    {
		struct capture* p = data;
        printf("result=%d, value=%d\\n", result, p->value);
        free(p);
    }, capture);
}

`;


sample["C2Y"]["Literal function async II"] =
    `
/*
   Pattern:
   do this -> then that -> then that ....
*/

#include <stdlib.h>
#include <stdio.h>

void login_async(void (* callback)(int id, void * data), void * data)
{
   callback(1, data);
}

void get_data_async(void (* callback)(const char* email, void * data), void * data)
{
  callback("your data...", data);
}

int main()
{
	struct capture { int id; }* capture = calloc(1, sizeof * capture);
    login_async((void (int id, void * p))
    {
        printf("login completed. id=%d\\n", id);
		struct capture * cap1 = p;
        cap1->id = id;
        get_data_async((void (const char* email, void * data))
        {
		    struct capture * cap2 = data;
            printf("your data='%s'  from id=%d\\n", email, cap2->id);
            free(cap2);
        }, cap1 /*MOVED*/);
    }, capture);
}



`;


sample["C2Y"]["Literal function scopes"] =
    `

void f1(){
    /*we cannot use local variables*/
    int i = 0;
    //(void(void)){ i = 1; }();
}

void f2(){
    /*we can use then at discarded expressions*/
    int i = 0;
    (void(void)){ int k = sizeof(i); }();
}

int g;
void f3(){
    /*we can use variables from file scope*/
    (void(void)){ int k = g; }();
}


void f4(){
    (void(void)){ const char * s = __func__; }();
}

`;

sample["C2Y"]["Literal function 1"] =
    `
#include <stdio.h>
int main()
{
  printf("%d", (int (void)){
    return 1;
  }());
}
`;

sample["C2Y"]["generic functions I"] =
    `
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE_MAX 10000

struct int_array {
    int* data;
    int size;
    int capacity;
};

#def reserve(A, N)
(int(typeof(A) p, int n)) {
    if (n > p->capacity) {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0])))) {
            return EOVERFLOW;
        }

        void* pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}
(A, N)
#enddef

#def push(A, I)
    (int(typeof(A) p, typeof(I) value)) {
    if (p->size == INT_MAX) {
        return EOVERFLOW;
    }

    if (p->size + 1 > p->capacity) {
        int new_capacity = 0;
        if (p->capacity > (INT_MAX - p->capacity / 2)) {
            /*overflow*/
            new_capacity = INT_MAX;
        } else {
            new_capacity = p->capacity + p->capacity / 2;
            if (new_capacity < p->size + 1) {
                new_capacity = p->size + 1;
            }
        }

        int error = reserve(p, new_capacity);
        if (error != 0) {
            return error;
        }
    }

    p->data[p->size] = value;

    p->size++;

    return 0;
}
(A, I)
#enddef

void int_array_destroy(struct int_array* p) {

    free(p->data);
}

int main() {
    struct int_array a = {0};
    push(&a, 1);
    int_array_destroy(&a);
}
`;

sample["C2Y"]["generic functions"] =
    `

#define SWAP(a, b)\\
  (void (typeof(a)* arg1, typeof(b)* arg2)) { \\
    typeof(a) temp = *arg1; *arg1 = *arg2; *arg2 = temp; \\
  }(&(a), &(b))


int main()
{
    int a = 1;
    int b = 2;
    SWAP(a, b);
    SWAP(a, b);
    SWAP(a, b);
}
`;



sample["C2Y"]["Statement expressions"] =
`

//https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3643.htm

#include <stdio.h>

#define maxint(a,b) \
  ({int _a = (a), _b = (b); _a > _b ? _a : _b; })

int main()
{
  printf("%d", maxint(1, 2));
}
`;



sample["Extensions"] = [];
sample["Extensions"]["try catch throw"] =
    `
#include <stdio.h>

int main()
{
  FILE * f = NULL;
  try
  {
     f = fopen("file.txt", "r");
     if (f == NULL) throw;

    /*success here*/
  }
  catch
  {
     /*some error*/
  }

  if (f)
    fclose(f);
}

`;


sample["Extensions"]["try catch throw II"] =
    `
#include <stdio.h>

/*not sure if usefull , but this is allowed*/

int main()
{
  try
  {
      FILE * f = NULL;
      try {
         FILE *f = fopen("file.txt", "r");
         if (f == NULL) throw;
         /*more*/
      }
      catch {
         if (f)
          fclose(f);
         throw;
      }
  }
  catch
  {
  }
}

`;

sample["Extensions"]["line slicing checks"] =
    `

#define M\\
ACRO 1

int main()
{
    const char* s = " asdas \\
    asdas";

    int a = \\
    1;
    
    //comment \\
    a = 2;

    /*
      ok
      path = c:\\path\\
   */
}

`;



sample["Extensions"]["pragma diagnostic"] =
    `

enum E1 { A };
enum E2 { B };

int main() {

#pragma CAKE diagnostic push
#pragma CAKE diagnostic error "C0004"
    if (A == B){}
#pragma CAKE diagnostic pop

#pragma CAKE diagnostic push
#pragma CAKE diagnostic warning "C0004"
    if (A == B){}
#pragma CAKE diagnostic pop

#pragma CAKE diagnostic push
#pragma CAKE diagnostic note "C0004"
    if (A == B){}
#pragma CAKE diagnostic pop


#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "C0004"
    if (A == B){}
#pragma CAKE diagnostic pop

}



`;

sample["Extensions"]["Traits"] =
    `

int main()
{
  int i;
  static_assert(_is_integral(i));
  static_assert(_is_floating_point(double) && _is_floating_point(float));
  static_assert(_is_function(main));

  char * p;
  static_assert(_is_scalar(p));
  static_assert(_is_scalar(nullptr));

  int a[10];
  static_assert(_is_array(a));

  /*pf = pointer to function (void) returning array 10 of int*/
  int (*pf)(void)[10];
  static_assert(!_is_array(pf));
  static_assert(_is_pointer(pf));

  

}
`;


sample["Extensions"]["assert"] =
    `
/* 
   assert in cake is a built-in expression.
   including assert.h if NDEBUG is defined, defines
   assert as ((void)0). Otherwise, assert is defined as assert(...) assert(__VA_ARGS__)
*/    

#define NDEBUG
#include <assert.h>

int main()
{
   assert(1 == 1);
}

`;

sample["Extensions"]["_Countof enum"] =
    `
#include <string.h>
enum E { A, B, C, D, E, F };

void f(enum E e)
{
    switch (e)
    {

        case A:
            break;
        case B:
            break;
        default:
            static_assert(_Countof(e) == 6);
    }
}

enum E parse_enum_e(const char* s)
{
    if (strcmp(s, "A") == 0) return A;
    if (strcmp(s, "B") == 0) return B;
    if (strcmp(s, "C") == 0) return C;
    if (strcmp(s, "D") == 0) return D;
    if (strcmp(s, "E") == 0) return E;
    if (strcmp(s, "F") == 0) return F;
    static_assert(_Countof(enum E) == 6);

    return A;
}

int main() { }
`;


sample["Extensions"]["MSVC __declspec"] =
    `
   //MSVC __declspec are parsed.
   //__declspec( selectany ) had to to implemented,
   //because the output depends on it.   
`;



sample["Extensions"]["__attribute__"] =
    `
   //GCC __attributes__ are parsed.
   //Cake implements C23 attributes and some GCC attributes could be
   //mapped but they are not at this moment.
`;

sample["Extensions"]["__builtin_offsetof"] =
    `

struct S {
    char c;
    double d;
};

int main(void)
{
    static_assert(__builtin_offsetof(struct S, c) == 0);
    static_assert(__builtin_offsetof(struct S, d) == 8);
}
`;

sample["Extensions"]["__builtin_c23_va_start"] =
    `
typedef __builtin_va_list va_list;

int add_nums_C23(int count, ...)
{
    int result = 0;
    va_list args;
    __builtin_c23_va_start(args);


    for (int i = 0; i < count; ++i) {
        result += __builtin_va_arg(args, int);
    }

    __builtin_va_end(args);
    return result;
}

int main(void)
{
    add_nums_C23(4 /*count*/, 25, 25, 50, 50);
}
`;

sample["Extensions"]["consteval"] =
    `

//syntax : const(constant-expression)
// This would be more useful when cake evaluate functions
// at compile time

int main(void)
{
    int i = 1;
    int j;
    j = const(5 + 3);

    // error
    // j = const(i + 3);
}
`;

sample["Static Analysis"] = [];

sample["Static Analysis"]["malloc/free"] =
    `
#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

int main() {
   void * _Owner _Opt p = malloc(1);
   free(p);
}

`;

sample["Static Analysis"]["static_state/static_debug"] =
    `
#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner ptr);

int main() {
   void * _Owner  _Opt p = malloc(1);
   if (p)
   {
     static_state(p, "not-null"); 
     free(p);
     static_state(p, "uninitialized"); 
   }
   static_state(p, "null | uninitialized"); 
   static_debug(p);
}

`;

sample["Static Analysis"]["implementing a destructor I"] =
    `
#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
  char *_Owner _Opt name;
};

void x_destroy(struct X x) 
{
  free(x.name);
}

int main() {
   struct X x = {0};
   x.name = strdup("a");
   x_destroy(x);
}


`;


sample["Static Analysis"]["implementing a destructor II"] =
    `
#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
  char *_Owner _Opt name;
};

void x_destroy(_Dtor struct X * x)
{
  free(x->name);
}

int main() {
   struct X x = {0};
   x.name = strdup("a");
   x_destroy(&x);
}

`;

sample["Static Analysis"]["ctor and dtor"] =
    `
#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
  char * _Owner _Opt text;
};

int init([[ctor]] struct X *p)
{ 
  //comment and see what happens
  p->text = strdup("a");
}

void destructor([[dtor]] struct X *p){
    //comment and see what happens
    free(p->text);    
}

int main() {   
  struct X x;
  init(&x);  
  destructor(&x);
}  

`;


sample["Static Analysis"]["_View qualifier"] =
    `
#pragma safety enable 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct X {
  char *_Owner _Opt name;
};

void f(_View struct X x) 
{
    if (x.name)
      printf(x.name);
}

int main() {
   struct X x = {0};
   x.name = strdup("a");
   f(x); /*not moved*/
   free(x.name);
}



`;



sample["Static Analysis"]["implementing delete"] =
    `
#pragma safety enable 

#include <stdlib.h>
#include <string.h>

struct X {
  char * _Owner _Opt text;
};

void x_delete(struct X * _Owner _Opt p)
{
    if (p)
    {
      free(p->text);
      free(p);    
    }
}

int main() {   
   struct X * _Owner _Opt p = calloc(1, sizeof * p);
   if (p){   
     p->text = strdup("a");     
   }
   x_delete(p);
 
}



`;


sample["Static Analysis"]["fix-me 1"] =
    `
#pragma safety disable

#include <stdlib.h>
#include <string.h>

struct X {
  char * text;
};

int main() {
   struct X x = {};
   x.text = strdup("a");
}
`;

sample["Static Analysis"]["Linked list"] =
    `#pragma safety enable

#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct item {
     char* _Owner title;
     struct item* _Owner _Opt  next;
};

struct list {
    struct item* _Owner _Opt head;
    struct item* _Opt tail;
};

void list_insert_after(struct list* list,
                       struct item* item,
                       struct item* _Owner p_new_item)
{
    if (item->next == NULL) {
        list->tail = p_new_item;
    }
    else {
        assert(p_new_item->next == NULL);
        p_new_item->next = item->next;
    }

    item->next = p_new_item;
}

void list_push_back(struct list* list,
                    struct item* _Owner p_item)
{
   if (list->head == NULL) {
      list->head = p_item;
   }
   else {
      assert(list->tail);
      assert(list->tail->next == nullptr);
      list->tail->next = p_item;
   }
   list->tail = p_item;
}

void list_push_front(struct list* list,
                    struct item* _Owner p_item)
{
    if (list->head == NULL) {
        list->tail = p_item;
    }
    else {
        assert(p_item->next == NULL);
        p_item->next = list->head;        
    }
    list->head = p_item;
}

void list_destroy(_Dtor struct list* list)
{
    struct item* _Owner _Opt it = list->head;
    while (it != NULL) {
        struct item* _Owner _Opt next = it->next;
        free(it->title);
        free(it);
        it = next;
    }
}

int main(int argc, char* argv[])
{
    struct list list = { 0 };
    struct item* _Owner _Opt p_item = calloc(1, sizeof(struct item));
    if (p_item)
    {
        char * _Owner _Opt title = strdup("title");
        if (title)
        {
           p_item->title = title;
           list_push_front(&list, p_item);
        }
        else
        {
            free(p_item);
        }
    }
    list_destroy(&list);
}
`;

sample["Static Analysis"]["dynamic array"] =
    `
#pragma safety enable

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

struct int_array {
    int* _Owner _Opt data;
    int size;
    int capacity;
};

int int_array_reserve(struct int_array* p, int n)
{
    if (n > p->capacity) {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0])))) {
            return EOVERFLOW;
        }

        void* _Owner _Opt pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;
        static_set(p->data, "moved");
        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int int_array_push_back(struct int_array* p, int value)
{
     if (p->size == INT_MAX) {
         return EOVERFLOW;
     }

     if (p->size + 1 > p->capacity) {
         int new_capacity = 0;
         if (p->capacity > (INT_MAX - p->capacity / 2))
         {
             /*overflow*/
             new_capacity = INT_MAX;
         }
         else {
             new_capacity =  p->capacity + p->capacity / 2;
             if (new_capacity < p->size + 1) {
                new_capacity = p->size + 1;
             }
         }

        int error = int_array_reserve(p, new_capacity);
        if (error != 0) {
            return error;
        }
     }

    p->data[p->size] = value;
    p->size++;

    return 0;
}

void int_array_destroy(_Dtor struct int_array* p)
{
    free(p->data);
}

int main()
{
   struct int_array a = { 0 };
   int_array_push_back(&a, 1);
   int_array_push_back(&a, 2);
   int_array_destroy(&a);
}
`;

sample["Static Analysis"]["using moved object"] =
    `
#pragma safety enable

#include <string.h>
#include <stdlib.h>

struct user
{
  int id;
  char* _Owner name;
};

void set_id(struct user* p, int id){}

int main()
{
  _Opt struct user user = {};
  user.name = strdup("a");
  char* _Owner name = user.name;
  free(name);
  set_id(&user, 1); //warning: object 'user.name' was moved  
}
`;

sample["Static Analysis"]["static_set/realloc"] =
    `
#pragma safety enable

void* _Owner _Opt realloc(void* _Opt ptr, unsigned size);
void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

void f()
{
    void * _Owner _Opt p = malloc(1);
    void * _Owner _Opt p2 = realloc(p, 2);
    if (p2 != 0)
    {
       /*overriding flow analysis*/
       static_set(p, "moved");
       p = p2;
    }
    free(p);
}

`;

sample["Static Analysis"]["mtx_t"] =
    `
#pragma safety enable

enum {
    mtx_plain ,
    mtx_timed,
    mtx_plain,
    mtx_timed,
};
};

enum {

    thrd_success , /* unspecified */
    thrd_nomem , /* unspecified */
    thrd_timedout , /* unspecified */
    thrd_busy , /* unspecified */
    thrd_error  /* unspecified */
};

typedef struct { _Owner int dummy; } mtx_t;
int mtx_init(_Out mtx_t *mtx, int type);
void mtx_destroy( _Dtor mtx_t * mutex );

int main()
{
   mtx_t mtx;
   if (mtx_init(&mtx, mtx_plain) != thrd_success)
   {
      static_set(mtx, "uninitialized");
      return;
   }
   mtx_destroy(&mtx);   
}

`;

sample["Static Analysis"]["socket"] =
    `
_Owner int socket();
void close(_Owner int fd);

int main()
{
  _Owner int fd;
  
  fd = socket();
  if (fd < 0)
  {
     static_set(fd, "null");
     return 1;
  }
  close(fd);
}
`;


sample["Static Analysis"]["owner assignment"] =
    `
#pragma safety enable

#include <string.h>
#include <stdlib.h>

int main()
{
  const char * _Owner _Opt s1 = strdup("hi");
  const char * _Owner _Opt s2 = NULL;

  s2 = s1; /*moved*/

  free(s2);
}

`;



sample["Static Analysis"]["takes_ownership"] =
    `
#pragma safety enable

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void takes_ownership(char * _Owner _Opt some_string)
{
    if (some_string)
    {
      printf("%s", some_string);
      free(some_string);
    }
}

int main()
{
    _Opt _Owner auto  s = strdup("hello");
    takes_ownership(s /*moved here*/ );
}

`;


sample["Static Analysis"]["gives ownership"] =
    `
#pragma safety enable

#include <string.h>
#include <stdlib.h>

const char * _Owner _Opt gives_ownership() {
    _Owner _Opt auto some_string = strdup("yours");
    return some_string;
}

int main(){
  _Owner _Opt auto s = gives_ownership();
  free(s);
}

`;

sample["Static Analysis"]["moving parts of _View"] =
    `
#pragma safety enable

#include <string.h>
#include <stdlib.h>

struct X {
  char * _Owner name;
};

struct Y {
  struct X x;
  struct X * px;
};


void x_destroy(_Dtor struct X * p)
{
  free(p->name);
}

void f(struct Y * p)
{
    x_destroy(&p->x);
    x_destroy(p->px);
}

int main() {
   struct Y  y = {};
   y.x.name = strdup("a");
   f(&y);
   free(y.x.name);
}

`;

sample["Static Analysis"]["_Owner pointer owns two objects"] =
    `
#pragma safety enable

void * _Owner _Opt calloc(unsigned long i, unsigned long sz);
char * _Owner _Opt strdup(const char* );
void free(void * _Owner _Opt p);

struct X {
  char *_Owner _Opt name;
};

int main()
{
   struct X * _Owner _Opt p = calloc(1, sizeof * p);
   if (p) {

     p->name = strdup("hi");
     struct X x = {0};
     x = *p;
     free(x.name);

     free(p);
   }
}

`;


sample["Static Analysis"]["checking double free"] =
    `
#pragma safety enable

void free(void * _Owner p);

struct X {
    char * _Owner naasdasdme;
    char * _Owner nasdasame;
    char * _Owner naasdme;
    char * _Owner namasde;
    char * _Owner namasade;
};

void x_destroy(_Dtor struct X * p)
{
    free(p->naasdasdme);
    free(p->nasdasame);
    free(p->naasdme);
    free(p->namasde);
    free(p->namasade);
    free(p->namasde);
}
`;

sample["Find the bug"] = [];
sample["Find the bug"]["Bug #1"] =
    `
#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
  char *_Owner _Opt name;
};

struct X f(int condition)
{
   struct X x;
   if (condition) {
        x.name = strdup("a");
   }
   return x;
}

int main()
{
    struct X x = f(2);
}

`;

sample["Find the bug"]["Bug #2"] =
    `

#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
  char *_Owner _Opt name;
};

void delete(struct X * _Owner _Opt p)
{
    if (p)
    {
       /*forgot to free p->name*/
       free(p);
    }
}

int main()
{
    struct X * _Owner _Opt p = calloc(1, sizeof * p);
    if (p)
    {
        p->name = strdup("a");
        delete(p);
    }
}

`;


sample["Find the bug"]["Bug #3"] =
    `
#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
  char *_Owner _Opt name;
  char *_Owner _Opt surname;
};

void delete(struct X * _Owner _Opt p)
{
    if (p)
    {
       free(p->name);
       /*forgot to free p->surname*/
       free(p);
    }
}

int main()
{
    struct X * _Owner _Opt p = malloc(sizeof * p);
    if (p)
    {
        p->name = strdup("a");
        /*p->surname is uninitialized*/
        delete(p);
    }
}

`;

sample["Find the bug"]["Bug #4"] =
    `

#pragma safety enable

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct X {
  char *_Owner _Opt name;
  char *_Owner _Opt surname;
};

void change(struct X * p)
{
     free(p->name);       
}

int main()
{
    struct X x = {0};
    x.name = strdup("a");
    change(&x);
    printf("%s", x.name);
    /*forgot to free x.name and x.surname*/
}

`;

sample["Find the bug"]["Bug #5"] =
    `


struct X
{
    int i;
};

void f(int condition)
{
   struct X * p = 0;
   if (condition)
   {
     struct X x = {};
     p = &x;
   }
   p->i = 1;
}
`;

sample["Find the bug"]["Bug #5"] =
    `
#pragma safety enable

void* _Owner _Opt calloc(unsigned int n, unsigned long size);
void free(void* _Owner _Opt ptr);

struct Y {
    int i; 
};

struct X {
    int i; 
    struct Y* _Opt pY;
};

int main() 
{
    struct X* _Owner _Opt pX = calloc(1, sizeof *pX);
    if (pX) 
    {
        struct Y* _Owner _Opt pY = calloc(1, sizeof *pY);
        if (pY) 
        {
            pX->pY = pY;
            struct X* _Opt p = pX;
            free(pY);            
            p->pY->i = 1;  // no warning            
        }
        free(pX);
    }
}

`;

sample["Find the bug"]["Bug #6"] =
    `
#pragma safety enable

#include <stdlib.h>

struct Obj2 {
    int i;
};

struct Obj {
    int i;
    struct Obj2* _Opt _Owner obj2;
};

void main() 
{
    struct Obj* _Opt _Owner o = calloc(1, sizeof *o);
    if (o) {
        o->obj2 = calloc(1, sizeof *o->obj2);
        if (o->obj2) {
            struct Obj* _Opt p = o;
            free(o->obj2);
            o->obj2 = nullptr;
            p->obj2->i = 1; //warning: object is possibly null 
        }
        free(o);
    }
}
`;

sample["Find the bug"]["Bug #7 K & R"] =
    `
/*
  This code is from 
  The C Programming Language 2 edition, page 145

  There are two bugs int this original sample
  - one memory leak
  - one invalid state
*/

#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct nlist {          /* table entry: */
    struct nlist *next; /* next entry in chain */
    char *name;  /* defined name */
    char *defn;  /* replacement text */
};

struct nlist *lookup(char *s);

/* hash:  form hash value for string s */
unsigned hash(char *s);


#define HASHSIZE 101

static struct nlist *hashtab[HASHSIZE]; /* pointer table */

/*1* lookup: look for s in hashtab */
struct nlist *lookup(char *);


/* install:  put (name, defn) in hashtab */
struct nlist *install(char *name, char *defn)
{
    struct nlist *np;
    unsigned hashval;

    if ((np = lookup(name)) == NULL) {  /* not found */
        np = (struct nlist *) malloc(sizeof(*np));
        if (np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        hashval = hash(name);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    } else      /* already there */
        free((void *) np->defn);  /* free previous defn */

    if ((np->defn = strdup(defn)) == NULL)
        return NULL;
    return np;
}
`;

sample["Find the bug"]["Bug #8"] =
    `
#pragma safety enable

void  f(int i)
{
    int k = 1;
    int * p = &k;
       
    if (i) {
       *p =0;
    }
    int h = 212/k;
}

int main() {}
`;

sample["Find the bug"]["Bug #9"] =
    `
enum E1 {A, B};
enum E2 {C, D};

void f(enum E1 e)
{
    switch(e)
    {
        case A:break;
        case D:break;
    }
}
int main(){}
`;

sample["Find the bug"]["Bug #10"] =
    `
#pragma safety enable

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct person {
    char* _Opt _Owner name;
};

void set(struct person* p, char* name) {
    free(p->name);
    char* _Opt _Owner temp = strdup(name);
    if (temp == NULL) return;
    p->name = temp;
    /*may leave this function with p->name uninitialized!*/
}

int main() {
    struct person p = {0};
    set(&p, "a");
    printf("%s", p.name);
    free(p.name);
    return 0;
}
`;

sample["Find the bug"]["Bug #11"] =
    `
#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);

struct X{
  char * p_ch;    
};

struct X * _Owner _Opt f()
{
   char ch  = 0;
   struct X * _Owner _Opt p = malloc(sizeof(struct X));
   if (p)
   {
       p->p_ch =  &ch;
       return p;
   } 
   return nullptr;   
}

`;

sample["cross compiling"] = [];
sample["cross compiling"]["target"] =
    `
/*
   Cake is a cross-compiling compiler, but unlike traditional compilers
   that produce machine executables directly, Cake generates C code as
   its output. This C code is tailored for the target platform, taking
   into account differences in architecture, data sizes, and calling conventions.
*/

#ifdef _WIN64
    // 64-bit Windows  -target=x64_msvc

    //long is 4 bytes on windows
    static_assert(sizeof(1L) == 4);
    static_assert(sizeof(void*) == 8);
    static_assert(sizeof(L""[0]) == 2);

#elif defined _WIN32

static_assert(sizeof(bool) == 1);
static_assert(alignof(bool) == 1);

static_assert(sizeof(char) == 1);
static_assert(alignof(char) == 1);

static_assert(sizeof(short) == 2);
static_assert(alignof(short) == 2);

static_assert(sizeof(L' ') == 2);
static_assert(sizeof(u' ') == 2);
static_assert(sizeof(U' ') == 4);

static_assert(sizeof(int) == 4);
static_assert(alignof(int) == 4);

static_assert(sizeof(long) == 4);
static_assert(alignof(long) == 4);

static_assert(sizeof(long long) == 8);
static_assert(alignof(long long) == 8);

static_assert(sizeof(float) == 4);
static_assert(alignof(float) == 4);

static_assert(sizeof(double) == 8);
static_assert(alignof(double) == 8);

static_assert(sizeof(long double) == 8);
static_assert(alignof(long double) == 8);

static_assert(sizeof(void*) == 4);
static_assert(alignof(void*) == 4);

static_assert(sizeof(nullptr) == 4);
static_assert(alignof(nullptr) == 4);

static_assert(sizeof(sizeof(0)) == 4);
static_assert(alignof(sizeof(0)) == 4);

static_assert(sizeof((char*)0-1) == 4);
static_assert(alignof((char*)0-1) == 4);



#endif

#ifdef __x86_64__

// linux -target=x86_x64_gcc

_Static_assert(sizeof(bool) == 1, "");
_Static_assert(alignof(bool) == 1, "");

_Static_assert(sizeof(char) == 1, "");
_Static_assert(alignof(char) == 1, "");

_Static_assert(sizeof(short) == 2, "");
_Static_assert(alignof(short) == 2, "");

_Static_assert(sizeof(L' ') == 4, "");
_Static_assert(sizeof(u' ') == 2, "");
_Static_assert(sizeof(U' ') == 4, "");

_Static_assert(sizeof(int) == 4, "");
_Static_assert(alignof(int) == 4, "");

_Static_assert(sizeof(long) == 8, "");
_Static_assert(alignof(long) == 8, "");

_Static_assert(sizeof(long long) == 8, "");
_Static_assert(alignof(long long) == 8, "");

_Static_assert(sizeof(float) == 4, "");
_Static_assert(alignof(float) == 4, "");

_Static_assert(sizeof(double) == 8, "");
_Static_assert(alignof(double) == 8, "");

_Static_assert(sizeof(long double) == 16, "");
_Static_assert(alignof(long double) == 16, "");

_Static_assert(sizeof(void*) == 8, "");
_Static_assert(alignof(void*) == 8, "");

_Static_assert(sizeof(nullptr) == 8, "");
_Static_assert(alignof(nullptr) == 8, "");

_Static_assert(sizeof(sizeof(0)) == 8, "");
_Static_assert(alignof(sizeof(0)) == 8, "");

_Static_assert(sizeof((char*)0 - 1) == 8, "");
_Static_assert(alignof((char*)0 - 1) == 8, "");


#endif


`;


sample["cross compiling"]["long"] =
    `

#include <stdio.h>
int main(void)
{
    printf("value=%ld", 2147483647L + 10);

    #ifdef _WIN32
    //use option : -target=x86_msvc
    static_assert(-2147483639 == 2147483647L + 10, "");
    #else
    //use option : -target=x86_x64_gcc
    static_assert(2147483657LL == 2147483647L + 10, "");
    #endif

    auto i32_max = 2'147'483'647;
    auto u32_max = 4'294'967'295;
    auto i64_max = 9'223'372'036'854'775'807;
    auto u64_max = 18'446'744'073'709'551'615;

}

`;


sample["cross compiling"]["integers"] =
    `


auto i32_max = 2'147'483'647;
auto i32_max1 = 2'147'483'648;

auto u32_max = 4'294'967'295;
auto u32_max1 = 4'294'967'296;

auto i64_max = 9'223'372'036'854'775'807;
auto i64_max1 = 9'223'372'036'854'775'808;
auto u64_max = 18'446'744'073'709'551'615;

`;



