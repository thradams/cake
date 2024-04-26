var sample = {};

sample["C99"] = [];
sample["C99"]["_Bool"] =
`
 /*
    _Bool type was introduced in C99 as built-in type used 
    to represent boolean values and the header <stdbool.h>
    with macros bool true and false.
    C23 introduced keywords (see C23 bool sample). 
 */

int main(void)
{
    // _Bool is converted to unsigned char. 

    _Bool b = 1;

    return 0;
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

sample["C99"]["int a[static]"] =
`
#include <stdlib.h>


void F(int a[static const 5]) 
{
    //cake was a warning when modifing array arguments
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

sample["C11"]=[];
sample["C11"]["_Generic"] =
    `
#include <math.h>

#define cbrt(X) _Generic((X), \\
                  double: cbrtl, \\
                  float: cbrtf ,\\
                  default: cbrtl  \\
              )(X)

#pragma expand cbrt


int main(void)
{
    cbrt(1.0);

    const int * const p;
    _Static_assert(_Generic(p, const int * : 1 ), "");
    _Static_assert(_Generic(&p, const int * const * : 1 ), "");
    _Static_assert(_Generic(main, int (*)(void) : 1 ), "");

    const int * const p2;
    static_assert(_Generic(p2, const int *: 1));

    static_assert(_Generic("abc",  char *: 1));    
}

`;

sample["C11"]["_Static_assert"] =
`
/*
   _Static_assert provides a mechanism for compile-time assertion 
   checking.

   Cake removed this statement when compiling to C99/C89.

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
* cake ouput source code is also utf8
*
* This web ouput also works with utf8. So everthing just works
* even without u8 prefix. (press compile ouput)
*
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

sample["C11"]["_Alignof / C23 alignof"] =
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

sample["C23"]=[]
sample["C23"]["Digit Separator"] =
`
#define M 1000'00

int main()
{
    int a = 1000'00;
    static_assert(1000'00 == 100000);
}

/*
  Note:
  This convesion happens at token level, even not active blocks
  are converted
*/
#if 0
#define X 100'00
#endif

`;

sample["C23"]["Binary Literal"] =
`
#define X  0b1010

int main()
{
    int a = X;
    int b = 0B1010;
}

/*
  Note:
  This convesion happens at token level, even not active blocks
  are converted
*/
`;


sample["C23"]["static_assert"] =
`
/*
   C23 added the alternative keyword static_assert for 
   _Static_assert.

   The error message also become optional in C23.

   Cake transpile static_assert to _Static_assert if the target 
   is C11 and removes static_assert if the target is C99/C89.
*/
int main()
{    
    static_assert(1 == 1, "error");
    
    /*message is optional in C23*/
    static_assert(1 == 1);
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
  unless for imediate compilation.
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

sample["C23"]["#warning"] =
    `
#include <stdio.h>

int main()
{
  /*
     We just comment this line when target is < c23
  */
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

#pragma expand SWAP

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

sample["C23"]["macro NEW"] =
`
#include <stdlib.h>
#include <string.h>

static inline void* allocate_and_copy(void* s, size_t n) {
    void* p = malloc(n);
    if (p) {
        memcpy(p, s, n);
    }
    return p;
}

#define NEW(...) (typeof(__VA_ARGS__)*) allocate_and_copy(&(__VA_ARGS__), sizeof(__VA_ARGS__))
#pragma expand NEW

struct X {
    const int i;
};

int main() { 
    auto p = NEW((struct X) {});     
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

#pragma expand SWAP

void f()
{
  int a = 1;
  int b = 2;
  SWAP(a, b);
}

auto literal_string = "hello auto";

struct {int i;} x;
auto x2 = x;

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
/*
  C23 introduced keyword bool as alternative to _Bool and 
  true and false as constants.
  
  Cake translate bool to _Bool when compiling to C99/C11
  and to unsigned char when compiling to C89.
*/

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

/*
  in case you want to add a compatibility header with
  nullptr defined as macro, then nullptr macro is preserved
  like this. The same for other features like static_assert.
*/

#define nullptr ((void*)0)

int F()
{
    void * p = nullptr;
    void * p2 = NULL;
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

int main()
{
    constexpr char ch = 'a';

    printf("%f %c", PI, ch);
}
`;

sample["C23"]["enum"] =
    `

enum X : short {
    A
};

int main() {
    enum X x = A;
}


`;

sample["C2Y"] = [];
sample["C2Y"]["if with initialization (Like C++17)"] =
    `
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


sample["C2Y"]["_Generic(type-name)"] =
`
/*
  cake accepts type-name on _Generic expression 
*/
int main(void)
{
    const int * const p;
    static_assert(_Generic(p, const int * : 1 ));
    static_assert(_Generic(typeof(p), const int * const: 1));
  
    static_assert(_Generic(int, int : 1));    
    static_assert(_Generic(typeof("abc"), char [4]: 1));
}

`;


sample["C2Y"]["defer inside try blocks"] =
    `
#include <stdio.h>

int main()
{

  try
  {
     FILE* f = fopen("in.txt", "r");
     if (f == NULL) throw;
     defer fclose(f);

     FILE* f2 = fopen("out.txt", "w");
     if (f2 == NULL) throw;
     defer fclose(f2);

     //...

    /*success here*/
  }
  catch
  {
     /*some error*/
  }


}

`;

sample["C2Y"]["defer with breaks III"] =
    `

#include <stdio.h>

int main()
{

  do
  {
     FILE* f = fopen("in.txt", "r");
     if (f == NULL) break;
     defer fclose(f);

     FILE* f2 = fopen("out.txt", "w");
     if (f2 == NULL) break;
     defer fclose(f2);

     //...

    /*success here*/
  }
  while(0);


}

`;


sample["C2Y"]["defer with breaks IV"] =
    `

#include <stdio.h>

int main()
{
  FILE* f = NULL;
  defer if (f) fclose(f);

  do
  {
     f = fopen("in.txt", "r");
     if (f == NULL) break;     
  }
  while(0);

}

`;


sample["C2Y"]["defer with return V"] =
    `

#include <stdio.h>

int main()
{
  FILE* f = fopen("in.txt", "r");
  if (f == NULL) return 1;
  defer fclose(f);

  FILE* f2 = fopen("out.txt", "w");
  if (f2 == NULL) return 1;
  defer fclose(f2);

  return 0;
}


`;


sample["C2Y"]["defer goto VI"] =
    `

#include <stdio.h>

int main()
{
  FILE* f = fopen("in.txt", "r");
  if (f != NULL)
  {
     defer fclose(f);

     FILE* f2 = fopen("out.txt", "w");
     if (f2 == NULL) goto LEND;
     defer fclose(f2);
  }
  LEND:
  return 0;
}

`;


sample["C2Y"]["defer and flow analysis"] =
    `
void f2(int i){}
void f(int k)
{
  int i;
  defer f2(i);
  
  if (k > 1)
   return;
}

`;


sample["C2Y"]["defer interleaved with return"] =
`

int f(){
  int i = 1;
  defer {
    i = 0;
  }
  return i++;
}

void f0(){
  int i = 1;
  defer {
    i = 0;
  }
  return; //empty or constant expression
}


`;


sample["Extensions"]=[];   
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



sample["Extensions"]["Literal function (lambda) I"] =
`
/*simple lambda*/
#include <stdio.h>
int main()
{
  printf("%d", (int (void)){
    return 1;
  }());
}
`;

sample["Extensions"]["Literal function (lambdas)"] =
    `
#include <stdio.h>
#include <stdlib.h>
extern char* strdup(const char* s);

void async(void* capture, int sz, void (*F)(void* data))
{
  /*
    The real function would copy capture and the function pointer
    to a queue and then execute at some thread pool
  */
  F(capture);
}

void dispatch(void* capture, int sz, void (*F)(void* data))
{
  /*
    The real function would copy capture and the function pointer
    to a queue and then execute sequencially.
  */
  F(capture);
}


void create_app(const char* appname)
{
  printf("main thread\\n");
 
  struct capture {
     char * name;
  } capture = { .name = strdup(appname) };

  async(&capture, sizeof capture, (void (void* p))
  {
    struct capture* capture = p;
    printf("this is running at any thread (name=%s)\\n", capture->name);

    dispatch(capture, sizeof *capture, (void (void* p) )
    {
      struct capture* capture = p;
      printf("back to main thread\\n");
      free(capture->name);
    });
  });
}

int main()
{
  create_app("string");
  return 0;
}

`;

sample["Extensions"]["typeof + lambdas"] =
`
/* Use -fo option to format output*/

#define SWAP(a, b)\\
  (void (typeof(a)* arg1, typeof(b)* arg2)) { \\
    typeof(a) temp = *arg1; *arg1 = *arg2; *arg2 = temp; \\
  }(&(a), &(b))

#pragma expand SWAP
int main()
{
    int a = 1;
    int b = 2;
    SWAP(a, b);
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
#pragma CAKE diagnostic error "-Wenum-conversion"
    if (A == B){}
#pragma CAKE diagnostic pop

#pragma CAKE diagnostic push
#pragma CAKE diagnostic warning "-Wenum-conversion"
    if (A == B){}
#pragma CAKE diagnostic pop

#pragma CAKE diagnostic push
#pragma CAKE diagnostic note "-Wenum-conversion"
    if (A == B){}
#pragma CAKE diagnostic pop


#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wenum-conversion"
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
   assert as ((void)0). Othewise, assert is defined as assert(...) assert(__VA_ARGS__)
*/    

#define NDEBUG
#include <assert.h>

int main()
{
   assert(1 == 1);
}

`;

sample["safe-mode"]=[];

sample["safe-mode"]["malloc/free"] =
`
#pragma ownership enable
#pragma nullable enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

int main() {
   void * _Owner _Opt p = malloc(1);
   free(p);
}

`;

sample["safe-mode"]["static_state/static_debug"] =
`
#pragma ownership enable
#pragma nullable enable

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

sample["safe-mode"]["implementing a destructor I"] =
`
#pragma ownership enable
#pragma nullable enable

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


sample["safe-mode"]["implementing a destructor II"] =
`
#pragma ownership enable
#pragma nullable enable

#include <stdlib.h>
#include <string.h>

struct X {
  char *_Owner _Opt name;
};

void x_destroy(struct X * _Obj_owner x) 
{
  free(x->name);
}

int main() {
   struct X x = {0};
   x.name = strdup("a");
   x_destroy(&x);
}

`;

sample["safe-mode"]["_View qualifier"] =
`
#pragma ownership enable 
#pragma nullable enable

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



sample["safe-mode"]["implementing delete"] =
`

#pragma ownership enable 
#pragma nullable enable

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


sample["safe-mode"]["fix-me 1"] =
`

//#pragma ownership enable 
//#pragma nullable enable

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

sample["safe-mode"]["Linked list"] =
`
#pragma ownership enable
#pragma nullable enable

#include <stdlib.h>
#include <assert.h>
#include <errno.h>

struct book {
     char* _Owner title;
     struct book* _Owner next;
};


struct books {
    struct book* _Owner head, *tail;
};

void books_insert_after(struct books* books, struct book* book, struct book* _Owner new_book)
{
    assert(books != NULL);
    assert(book != NULL);
    assert(new_book != NULL);
    assert(new_book->next == NULL);


    if (book->next == NULL) {
        books->tail = new_book;
    }
    else {
        new_book->next = book->next;
    }

    book->next = new_book;
}



void books_push_back(struct books* books, struct book* _Owner new_book)
{
   assert(books != NULL);
   assert(new_book != NULL);
   assert(new_book->next == NULL);

   if (books->head == NULL) {
      books->head = new_book;
   }
   else {
      books->tail->next = new_book;
   }
   books->tail = new_book;
}

void books_push_front(struct books* books, struct book* _Owner new_book)
{
    assert(books != NULL);
    assert(new_book != NULL);
    assert(new_book->next == NULL);

    if (books->head == NULL) {
        books->tail = new_book;
    }
    else {
        new_book->next = books->head;        
    }
    books->head = new_book;
}

void books_destroy(struct books* _Obj_owner books)
{
    //pre condition
    assert(books != NULL);

    struct book* _Owner it = books->head;
    while (it != NULL) {
        struct book* _Owner next = it->next;
        free(it->title);
        free(it);
        it = next;
    }
}

int main(int argc, char* argv[])
{
    struct books list = { 0 };
    struct book* _Owner b1 = calloc(1, sizeof(struct book));
    if (b1)
    {
        books_push_front(&list, b1);
    }
    books_destroy(&list);
}

`;

sample["safe-mode"]["dynamic array"] =
`

#pragma ownership enable
#pragma nullable enable

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

void int_array_destroy(struct int_array* _Obj_owner p)
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

sample["safe-mode"]["using moved object"] =
`
#pragma ownership enable
#pragma nullable enable

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
  struct user user = {};
  user.name = strdup("a");
  char* _Owner name = user.name;
  free(name);
  set_id(&user, 1); //warning: object 'user.name' was moved  
}
`;

sample["safe-mode"]["static_set/realloc"] =
`

void* _Owner realloc(void* ptr, unsigned size);
void* _Owner malloc(unsigned long size);
void free(void* _Owner ptr);

void f()
{
    void * _Owner p = malloc(1);
    void * _Owner p2 = realloc(p, 2);
    if (p2 != 0)
    {
       /*
          if p2 != 0 it  means p was moved.
       */
       static_set(p, "moved");
       p = p2;
    }    
    free(p);
}


`;


sample["flow-analysis"] = [];
sample["flow-analysis"]["if-else 1"] =
`

void* _Owner malloc(unsigned long size);
void free(void* _Owner ptr);

void f1()
{
    void * _Owner p = malloc(1);
    if (p) {
      static_state(p, "not-null");
    }

    static_state(p, "null | not-null");
    free(p);
}

void f2(int condition)
{
    void * _Owner p = malloc(1);
    if (condition) {
      static_state(p, "null | not-null");
    }

    static_state(p, "null | not-null");
    free(p);
}

void f3(int condition)
{
    void * _Owner p = malloc(1);
    
    if (condition) {
       free(p);
    }
    else {
       free(p);
    }

    static_state(p, "uninitialized");    
}

void f3(int condition)
{
    void * _Owner p = malloc(1);
    
    if (condition) {
       
    }
    else {
       free(p);
    }

    static_state(p, "uninitialized | maybe_null");    
}


void f4(int condition)
{
    void * _Owner p = malloc(1);
    
    if (condition) {
       free(p);
    }
    else {
       
    }

    static_state(p, "uninitialized | maybe_null");    
}

void f5(int condition)
{
    void * _Owner p = malloc(1);
    
    if (p) {
       free(p);
       return;
    }
    
    static_state(p, "null");    
}

`;

sample["flow-analysis"]["while"] =
`    
void * f();
void * next(void* p);

void f1()
{
    void * p = f();
    static_state(p, "null | not-null");

    while (p) {
      p = next(p);
    }
    static_state(p, "null");
}
`;


sample["flow-analysis"]["switch"] =
`    
    
void * f();


void f1(int condition)
{
    void * p = f();    
    switch (condition)
    {
        case 1:
            p = 0;
        break;
    }
    static_state(p, "null | not-null");
}


void f2(int condition)
{
    void * p = f();    
    switch (condition)
    {
        case 1:
          p = 0;
        break;
      default:
         p = 0;
        break;
    }
    static_state(p, "null");
}

`;


sample["safe-mode"]["mtx_t"] =
`
enum {
    mtx_plain ,
    mtx_timed,
    mtx_plain,
    mtx_timed,
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
void mtx_destroy( mtx_t * _Obj_owner mutex );

int main()
{
   mtx_t mtx;
   if (mtx_init(&mtx, mtx_plain) == thrd_success)
   {
      static_set(mtx, "not-null");
      mtx_destroy(&mtx);
   }
}
`;

sample["safe-mode"]["socket"] =
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


sample["safe-mode"]["assignment"] =
`

#pragma ownership enable
#pragma nullable enable

#include <string.h>
#include <stdlib.h>

int main()
{
  const char * _Owner _Opt s1 = strdup("hi");
  const char * _Owner _Opt s2 = NULL;

  s2 = s1;

  free(s2);
}

`;



sample["safe-mode"]["takes_ownership"] =
`

#pragma ownership enable
#pragma nullable enable

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
    takes_ownership(s);
}

`;


sample["safe-mode"]["gives ownership"] =
`

#pragma ownership enable
#pragma nullable enable

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

sample["safe-mode"]["moving parts of _View"] =
`
#pragma ownership enable
#pragma nullable enable

#include <string.h>
#include <stdlib.h>


struct X {
  char * _Owner name;
};

struct Y {
  struct X x;
  struct X * px;
};


void x_destroy(struct X * _Obj_owner p) 
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

sample["safe-mode"]["_Owner pointer owns two objects"] =
`
#pragma ownership enable
#pragma nullable enable


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


sample["safe-mode"]["checking double free"] =
`
void free(void * _Owner p);

struct X {
    char * _Owner naasdasdme;
    char * _Owner nasdasame;
    char * _Owner naasdme;
    char * _Owner namasde;
    char * _Owner namasade;
};

void x_destroy(struct X * _Obj_owner p)
{
    free(p->naasdasdme);
    free(p->nasdasame);
    free(p->naasdme);
    free(p->namasde);
    free(p->namasade);
    free(p->namasde);
}
`;

sample["find the bug"] = [];
sample["find the bug"]["Bug #1"] =
`

#pragma ownership enable
#pragma nullable enable

#include <stdlib.h>
#include <string.h>

struct X {
  char *_Owner name;
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

sample["find the bug"]["Bug #2"] =
`

#pragma ownership enable
#pragma nullable enable

#include <stdlib.h>
#include <string.h>

struct X {
  char *_Owner name;
};

void delete(struct X * _Owner p)
{
    if (p)
     free(p);
}

int main()
{
    struct X * _Owner p = calloc(1, sizeof * p);
    if (p)
    {
        p->name = strdup("a");
        delete(p);
    }
}

`;


sample["find the bug"]["Bug #3"] =
    `

#pragma ownership enable
#pragma nullable enable

#include <stdlib.h>
#include <string.h>

struct X {
  char *_Owner name;
  char *_Owner surname;
};

void delete(struct X * _Owner p)
{
    if (p)
    {
       free(p->name);
       free(p);
    }
}

int main()
{
    struct X * _Owner p = malloc(sizeof * p);
    if (p)
    {
        p->name = strdup("a");
        delete(p);
    }
}


`;

sample["find the bug"]["Bug #4"] =
`

#pragma ownership enable
#pragma nullable enable

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct X {
  char *_Owner name;
  char *_Owner surname;
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
}

`;

sample["find the bug"]["Bug #5"] =
`
#pragma nullable enable

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

sample["find the bug"]["Bug #5"] =
`
#pragma ownership enable
#pragma nullable enable

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
