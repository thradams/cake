
## Using cake

Cake works as an extension for MSVC on Windows and as an extension for GCC on Linux.
This approach makes Cake useful in real and existing programs. 

When applicable, Cake uses the same command line options of MSVC and GCC.

### Include directories

Include directories are specified in `cakeconfig.h` file.

On Windows, to manually discover which directories are included, you can run at 
Visual Studio command prompt the command:


```
echo %INCLUDE%
```

To find out what are the directories used by GCC type:

```
echo | gcc -E -Wp,-v -
```
  
Sample of `cakeconfig.h`

```c

#ifdef __linux__
/*
   To find the include directories used my GCC type:   
   echo | gcc -E -Wp,-v -
*/
#pragma dir "/usr/lib/gcc/x86_64-linux-gnu/11/include"
#pragma dir "/usr/local/include"
#pragma dir "/usr/include/x86_64-linux-gnu"
#pragma dir "/usr/include"

#endif

#ifdef _WIN32
/*
   To find the include directories used my  MSVC,
   open Visual Studio Developer Commmand prompt and type:
   echo %INCLUDE%.
   Running Cake inside mscv command prompt uses %INCLUDE% automatically.
*/
#pragma dir "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.38.33130/include"
#pragma dir "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.38.33130/ATLMFC/include"
#pragma dir "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/VS/include"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/ucrt"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/um"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/shared"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/winrt"
#pragma dir "C:/Program Files (x86)/Windows Kits/10/include/10.0.22000.0/cppwinrt"
#pragma dir "C:/Program Files (x86)/Windows Kits/NETFXSDK/4.8/include/um"

#endif

```

The command line `cake -autoconfig` generates the cake config file.

We can have a `cakeconfig.h` per project and call a more generic `cakeconfig.h` for system includes.

Sample: 

`yourproject\cakeconfig.h`

```c

//system includes...etc
#include "C:\Program Files (x86)\cake\cakeconfig.h"

//project extra includes
#pragma dir ".\openssl\include"

```


### Command line


```
cake [options] source1.c source2.c ...

SAMPLES

    cake source.c
    Compiles source.c and outputs /[default-target]/source.c

    cake -target=X86_msvc source.c
    Compiles source.c and outputs C11 code at /X86_msvc/source.c

    cake file.c -o file.cc && cl file.cc
    Compiles file.c and outputs file.cc then use cl to compile file.cc

  
```

### Options

 * `-I`  (same as GCC and MSVC)
Adds a directory to the list of directories searched for include files

 * `-no-output`
Cake will not generate output

* `-D` (same as GCC and MSVC)
Defines a preprocessing symbol for a source file

* `-E` (same as GCC and MSVC)
Copies preprocessor output to standard output

* `-o name.c` (same as GCC and MSVC)
  Defines the output name, when we compile a single file

* `-dump-tokens`
Output tokens before preprocessor

* `-Wnumber -Wno-number`
Enables or disable warnings.
See [warnings](warnings.html)

* `-disable-assert`
Disable cake extension where assert is an statement.

* `-H` (same as gcc, /showIncludes in MSVC)
Causes the compiler to output a list of the include files. 

* `*-preprocess-def-macro`
preprocess def macros after expansion

* `-Wall`
Enables all warnings

* `-sarif`
Generates sarif files.
Sarif Visual Studio plugin https://marketplace.visualstudio.com/items?itemName=WDGIS.MicrosoftSarifViewer

* `-sarif-path`
Specifies the Sarif output dir. "Visual Studio -> External Tools" 
`-Wstyle  -msvc-output  -no-output -sarif -sarif-path "$(SolutionDir).sarif" $(ItemPath)`

*  `-target`
Defines how the source code is interpreted (integers sizes, align etc) and specifies the
C89 output that is compatible with the target compiler.
Options: x86\_x64_gcc, x64\_msvc, x64\_msvc, catalina, ccu8

*  `-msvc-output` Output is compatible with Visual Studio IDE. 

*  `-fdiagnostics-color=never` (same as GCC) Output will not use colors

*  `-fanalyzer` runs cake flow analysis

* `-auto-config` Generates cakeconfig.h header (see includes)

* `-style=name` Set the style used in (w011) style warnings. Options are `-style=cake`, `-style=gnu`, `-style=microsoft`

* `-comment-to-attr` Converts at preprocessor phase, comment like this `/*w12*/` to attributes `[[cake::w12]]`
 
* `-const-literal` Makes the compiler handle string literals as const char[] rather than char[].

### Output

The current backend generates C89-compatible code, which can be pipelined with existing 
compilers to produce executables. 

The output is a simplified version of C89.
It does not include the following features:

 - preprocessor
 - typedefs
 - enums
 - const
 - auto
 - Structs/unions declared inside other structs/unions
 - constant expressions are pre-computed
 - no sizeof
 - static variables are non-local.
 - arrays size [] = {...} are pre-calculated
 - no switch 
 
 
The goal is for this simplified version to function as an intermediate language (IL).

One directory called **out** is created keeping the same directory structure of the input files.

For instance:

```c
cake c:\project\file1.c
```

output:

```
  c:\project
  ├── file1.c
  ├── target
      ├── file1.c
```

More files..

```
cake c:\project\file1.c c:\project\other\file2.c
```

output

```
  c:\project
  ├── file1.c
  ├── other
  │   ├── file2.c
  ├── target
      ├── file1.c
      ├── other
          ├── file2.c
```

## C89

https://nvlpubs.nist.gov/nistpubs/Legacy/FIPS/fipspub160.pdf
## C99

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n325.pdf
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf


```c
 #define __STDC_VERSION__ 199901L  //C99
```

### C99 restrict pointers

```c
void f(const char* restrict s);
int main(){
    f("");
}
```

<button onclick="Try(this)">try</button>

Currently restrict is being removed on the generated code.


###  C99 Variable-length array (VLA) 


```c
#include <stdlib.h>
#include <stdio.h>

int main() {
	int n = 2;
	int m = 3;
	int (*p)[n][m] = malloc(sizeof * p);

	printf("%zu\n", sizeof(*p));

	free(p);
}

```
<button onclick="Try(this)">try</button>

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n683.htm

Not implemented yet.

### C99 Flexible array members

```c

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

```

<button onclick="Try(this)">try</button>


### C99 static and type qualifiers in parameter array declarators

```c
#include <stdlib.h>

void F(int a[static 5]) {
}

int main() 
{    
    F(0);
    F(NULL);
    F(nullptr);

    int a[] = {1, 2, 3};    
    F(a);//error
    
    int b[] = { 1, 2, 3 , 4, 5};
    F(b); 

    int c[] = { 1, 2, 3 , 4, 5, 6};
    F(c);
}

```
<button onclick="Try(this)">try</button>

Cake will perform the same checks regardless of the static keyword.

### C99 Complex and imaginary support
Not implemented

### C99 Universal character names (\u and \U)
Not implemented

### C99 Hexadecimal floating constants

```c
double d = 0x1p+1;
```
  <button onclick="Try(this)">try</button>


Cake converts hexadecimal floating-point values to decimal 
floating-point representation using strtod followed by snprintf.
This conversion may introduce precision loss.


```
0x1.234p1 means:

           2       3     4
r1= 1 +   ---  +  --- + ---   = 1.1376953125 
            1        2     3
          16       16     16

                1
1.1376953125 x 2  = 2.275390625 (final number)

```


### C99 Compound literals

```c
struct s {
  int i;
};

int f(void) {
  struct s * p = 0, * q;
  int j = 0;
  again:
    q = p, p = & ((struct s) { j++ });
  if (j < 2) goto again;
  return p == q && q -> i == 1;
}
```
<button onclick="Try(this)">try</button>


N716
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n716.htm

### C99 Designated initializers

```c
 int main()
 {
  int a[6] = {[4] = 29, [2] = 15 };

  struct point { int x, y; };

  struct point p = { .y = 2, .x = 3 };
 }

```

<button onclick="Try(this)">try</button>

N494
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n494.pdf

### C99 Line comments

```c

//line comments

```

### Declarations in for loop initializers 


```c
int main()
{
   const int max = 10;
   for (int n = max - 1; n >= 0; n--)
   {
     // body of loop
   }
}

```

<button onclick="Try(this)">try</button>

### C99 inline functions

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n709.htm  (30 May 1997)
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n741.htm


```c
inline int sum(int a, int b)
{
    return a + b;
}
int main(void)
{
    int r = sum(1, 2);
}
```

<button onclick="Try(this)">try</button>

### C99 _Pragma preprocessing operator

```c

// Use -E option
//6.10.11 Pragma operator

#define LISTING(x) PRAGMA(listing on #x)
#define PRAGMA(x) _Pragma(#x)

LISTING (..listing.dir)


```
<button onclick="Try(this)">try</button>



### C99 \_\_func\_\_ predefined identifier

N611 13 Sep 96 Mooney, \_\_FUNC\_\_

```c
#include <stdio.h>
int main()
{
    printf("%s\n", __func__);
    printf("%s\n", __func__);
}
```

<button onclick="Try(this)">try</button>

###  C99 Variadic macros


```c

#include <stdio.h>

#define debug(...) fprintf(stderr, __VA_ARGS__)

int main()
{
  int x = 1;
  debug("X = %d\n", 1);
}
```

<button onclick="Try(this)">try</button>


N707
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n707.htm


###  C99 \_Bool

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n815.htm  (1998)

```c
int main(void)
{
    _Bool b = 1;
    return 0;
}
```

<button onclick="Try(this)">try</button>

## C11

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2310.pdf


```c
#define __STDC_VERSION__ 201112L //C11
```

###  C11 \_Static\_assert

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1330.pdf

```c
int main()
{
    _Static_assert(1 == 1, "error");    
}
```

<button onclick="Try(this)">try</button>

\_Static\_assert became static_assert in C23.

### C11 Anonymous structures and unions

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1406.pdf


```c
struct v {
  union { /* anonymous union*/
     struct { int i, j; }; /* anonymous structure*/
     struct { long k, l; } w;
  };
  int m;
} v1;

int main(){
  v1.i = 2; /* valid*/
  v1.w.k = 5; /* valid*/
}
```
<button onclick="Try(this)">try</button>




### C11 \_Noreturn

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1478.htm

```c
_Noreturn void f () {
  abort(); // ok
}
```

`_Noreturn` became `[[noreturn]]` in C23.

<button onclick="Try(this)">try</button>


###  C11 Thread_local/Atomic


Thread_local uses __declspec(thread) in MSVC output and __thread with GCC output.

Atomic - not implemented

###  C11 type-generic expressions (\_Generic)

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1441.htm

```c
#include <math.h>

#define cbrt(X) _Generic((X),    \
                  double: cbrtl, \
                  float: cbrtf , \
                  default: cbrtl \
              )(X)


int main(void)
{
    cbrt(1.0);
}
```
<button onclick="Try(this)">try</button>


###  C11 u' ' U' ' character constants

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1326.pdf

```c
 int i = U'ç';
 int i2 = u'ç';
```
<button onclick="Try(this)">try</button>

Important: Cake assume source is utf8 encoded.

###  C11 u8"literals"

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1488.htm

```c
char * s1 = u8"maçã";
char * s2 = u8"maca";
```
<button onclick="Try(this)">try</button>

Important: Cake assume source is utf8 encoded.

### C11 _Alignof or C23 alignof

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1397.htm

```c
 int main()
 {
   int align = alignof(int);
 }
```
<button onclick="Try(this)">try</button>

`_Alignof` became `alignof` in C23.

### C11 _Alignas or C23 alignas

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1335.pdf

Uses `__declspec(align(n))` in MSVC output and `__attribute__((aligned(n)))` in GCC output.


## C23

https://open-std.org/JTC1/SC22/WG14/www/docs/n3096.pdf

```c
#define __STDC_VERSION__ 201710L  //C17
#define __STDC_VERSION__ 202311L  //C23
```

###  C23 \_Decimal32, \_Decimal64, and \_Decimal128

Not implemented.
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1107.htm

### C23 static\_assert / single-argument static_assert

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1330.pdf

```c
int main(void)
{
    static_assert(1 == 2);
}
```

<button onclick="Try(this)">try</button>

###  C23 u8 character prefix

https://open-std.org/JTC1/SC22/WG14/www/docs/n2418.pdf

```c
int main(){
    unsigned char c = u8'~';
}
```
<button onclick="Try(this)">try</button>

### C23 No function declarators without prototypes

https://www.open-std.org/JTC1/SC22/WG14/www/docs/n2841.htm

```c
int main(){
    func(); //this is an error in C23
}
```
<button onclick="Try(this)">try</button>

See also Remove support for function definitions with identifier lists  

https://open-std.org/JTC1/SC22/WG14/www/docs/n2432.pdf


### C23 Improved Tag Compatibility

Not implemented yet.
  
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3037.pdf
  
```c
struct foo { int a; } p;
void bar(void)
{
  struct foo { int a; } q;
  q = p;
}
```
<button onclick="Try(this)">try</button>


### C23 Unnamed parameters in function definitions

https://open-std.org/JTC1/SC22/WG14/www/docs/n2480.pdf

```c
int f(int );

int f(int ) {
}
```
<button onclick="Try(this)">try</button>

Missing a dummy name when generating c89.

### C23 Digit separators

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2626.pdf

```c
int main()
{
    int a = 1000'00;
}
```
<button onclick="Try(this)">try</button>


### C23 Binary literals 

<button onclick="Try(this)">try</button>


```c
#define X  0b1010

int main()
{
    int a = X;
    int b = 0B1010;
}

```


### C23 Introduce the nullptr constant

https://open-std.org/JTC1/SC22/WG14/www/docs/n3042.htm

```c

int main()
{
  void * p = nullptr;
  auto p2 = nullptr;
  typeof(nullptr) p3 = nullptr;
}

```
<button onclick="Try(this)">try</button>


### C23 Make false and true first-class language features

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2935.pdf

```c

#if true
#warning yes..
#endif

int main()
{
    bool b = true;
}
```

<button onclick="Try(this)">try</button>

###  C23 {} empty initializer

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2900.htm
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3011.htm


```c

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

```
<button onclick="Try(this)">try</button>

###  C23 auto

https://open-std.org/JTC1/SC22/WG14/www/docs/n3007.htm

```c
static auto a = 3.5;
auto p = &a;

double A[3] = { 0 };
auto pA = A;
auto qA = &A;
```

<button onclick="Try(this)">try</button>


###  C23 typeof / typeof_unqual

https://open-std.org/JTC1/SC22/WG14/www/docs/n2927.htm
https://open-std.org/JTC1/SC22/WG14/www/docs/n2930.pdf

```c

#define SWAP(a, b) \
  do {\
    typeof(a) temp = a; a = b; b = temp; \
  } while (0)


int main()
{
    /*simple case*/
    int a = 1;
    typeof(a) b = 1;

    /*pay attention to the pointer*/
    typeof(int*) p1, p2;

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
   typeof(void (int)) * pf = nullptr;
}

```
  
<button onclick="Try(this)">try</button>


### C23 Improved Normal Enumerations

//TODO

https://open-std.org/JTC1/SC22/WG14/www/docs/n3029.htm

```c
enum a {
	a0 = 0xFFFFFFFFFFFFFFFFULL
};

static_assert(_Generic(a0,
		unsigned long long: 0,
		int: 1,
		default: 2 == 0));
```

<button onclick="Try(this)">try</button>

The type of the enum must be adjusted.

###  C23 constexpr 

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3018.htm


```c

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


```
<button onclick="Try(this)">try</button>
  
###  C23 Enhancements to Enumerations

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3030.htm

```c
enum X : short {
  A
};

int main() {
   enum X x = A;   
}
```
<button onclick="Try(this)">try</button>


###  C23 [[Attributes]]

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2335.pdf
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2554.pdf

### C23 [[fallthrough]] attribute
TODO

https://open-std.org/JTC1/SC22/WG14/www/docs/n2408.pdf

```c
void g(){}
void h(){}
void i(){}
 
void f(int n) {
    void g(void), h(void), i(void);
    switch (n) {
        case 1: /* diagnostic on fallthrough discouraged */
        case 2:
            g();
            [[fallthrough]];
        case 3: /* diagnostic on fallthrough discouraged */
            h();
        case 4: /* fallthrough diagnostic encouraged */
            i();
            //[[fallthrough]]; /* constraint violation */            
    }
}
```
<button onclick="Try(this)">try</button>

### C23 [[deprecated]] attribute

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2334.pdf

```c
// Compil with -w03
[[deprecated]] void f2() {}
struct [[deprecated]] S {  int a;};
enum [[deprecated]] E1 { one };

int main(void) {
    struct S s;
    enum E1 e;
    f2();
}
```

<button onclick="Try(this)">try</button>

### C23 [[maybe_unused]] attribute

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2270.pdf

```c
//
// Compile with -w02 -w06
//
void f( [[maybe_unused]] int arg1, int arg2) //warning C0006: 'arg2': unreferenced formal parameter
{
    [[maybe_unused]] int local1;
    int local2; //warning C0002: 'local2': unreferenced declarator
}
```

<button onclick="Try(this)">try</button>

### C23 [[nodiscard]] attribute

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2267.pdf

https://open-std.org/JTC1/SC22/WG14/www/docs/n2448.pdf

```c
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

```

<button onclick="Try(this)">try</button>

Cake implementation is missing the message.

### C23 [[unsequenced]] and [[reproducible]]

TODO

https://open-std.org/JTC1/SC22/WG14/www/docs/n2956.htm

```c
typedef double f_t [[reproducible]] (double);    // invalid, applies to identifier f_t
typedef double g_t(double) [[reproducible]];     // valid, applies to type
extern g_t f [[unsequenced]];                    // invalid, applies to identifier f
extern typeof(double(double)) [[unsequenced]] g; // valid, applies to type specifier
extern g_t [[unsequenced]] g;                    // valid, applies to type specifier
```

###  C23 \_\_has\_attribute

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2799.pdf

```c

#if __has_c_attribute(fallthrough)
#warning YES
#else
#warning NO
#endif
```
<button onclick="Try(this)">try</button>


###  C23 \_\_has\_include

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2799.pdf

```c

#if __has_include(<stdio.h>)
#warning  YES
#endif

#if __has_include(<any.h>)
#warning  YES
#else
#warning  NO
#endif

```
<button onclick="Try(this)">try</button>


###  C23 \#warning

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2686.pdf

```c
int main()
{
  #warning my warning message  
}
```
<button onclick="Try(this)">try</button>


###  C23 #embed

Partially implemented.

```c
#include <stdio.h>

int main()
{
  static const char file_txt[] = {
   #embed "stdio.h"
   ,0
  };

  printf("%s\n", file_txt);
}
```
<button onclick="Try(this)">try</button>


###  C23 \#elifdef \#elifndef

<button onclick="Try(this)">try</button>

```c
#define Y

#ifdef X
#define VERSION 1
#elifdef  Y
#define VERSION 2
#else
#define VERSION 3
#endif
```

  
###  C23 \_\_VA_OPT\_\_

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3033.htm

```c

#define F(...) f(0 __VA_OPT__(,) __VA_ARGS__)
#define G(X, ...) f(0, X __VA_OPT__(,) __VA_ARGS__)
#define SDEF(sname, ...) S sname __VA_OPT__(= { __VA_ARGS__ })
#define EMP


void f(int i, ...) {}


int main()
{
  int a = 1;
  int b = 2;
  int c = 3;
  
  F(a, b, c);
  F();
  F(EMP);
  G(a, b, c);
  G(a, );
  G(a);

}

```

<button onclick="Try(this)">try</button>


###  C23 BitInt(N))

TODO 
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2763.pdf

```c
int main()
{
    _BitInt(2) a2 = 1;
    _BitInt(3) a3 = 2;
    _BitInt(33) a33 = 1;
    char c = 3;
}
```
<button onclick="Try(this)">try</button>


### C23 Compound Literals with storage specifier
  
TODO

```c
void F(int *p){}

int main()
{
   F((static int []){1, 2, 3, 0})
}
```
<button onclick="Try(this)">try</button>

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3038.htm

### C23 Variably-modified (VM) types

TODO

Variably modified types part of C23. 
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2778.pdf

Only variable length arrays with automatic storage duration that are optional.

## C2Y

### C2Y Obsolete implicitly octal literals

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3353.htm

```c

static_assert(0o52 == 052);
static_assert(0O52 == 052);
static_assert(0O52 == 42);

int main()
{
    int i = 0o52;
}

```
<button onclick="Try(this)">try</button>


### C2Y case range expressions

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3370.htm

```c
  #include <stdio.h>

  void f(int n)
  {
    switch (n)
    {
       case 1 ... 10:
       printf("n in range 1...10\n");
       break;
       default:
       break;
    }
  }

  int main(){
    f(1);
    f(11);
  }
 ```

### C2Y #def

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3524.txt

```c
//Use -E
#def foo(x)
		do {
			bar(x);
			baz(x);
		}
        while (0)
#enddef

foo(1)
foo(2)
```

<button onclick="Try(this)">try</button>

It may not be part of C2Y, but it is implemented in Cake while 
we wait to see whether we will keep it as an extension or remove it.


### C2Y \_Countof operator 

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3369.pdf

```c
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

```

<button onclick="Try(this)">try</button>

Obs: Cake extends countof to enums, returning the number
of enumerators. (this is not part of C2Y)
It can be a new keyword in cake.

```c
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
```
<button onclick="Try(this)">try</button>

### C2Y defer

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3734.pdf

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3733.htm


```c
// 12 EXAMPLE 1 Defer statements cannot be jumped over.
#include <stdio.h>

int main() 
{    
    goto target;  // constraint violation
 
    _Defer { fputs(" meow", stdout); }
    
    target:

    fputs("cat says", stdout);
    return 1;
}
```

<button onclick="Try(this)">try</button>

```c

/*g*/

#include <stdio.h>

int main()
{
    // print "cat says" to standard output
    return fputs("cat says", stdout);

    _Defer { fputs(" meow", stdout); }  // okay: no constraint violation,

    // not executed
}

```

<button onclick="Try(this)">try</button>

```c
/*h*/

#include <stdio.h>
int main()
{    
    goto target;
    {
        // okay: no constraint violation
        _Defer { fputs(" meow", stdout); }
    }

    target:

    fputs("cat says", stdout);
    return 1;  // prints "cat says" to standard output
}
```
<button onclick="Try(this)">try</button>

```c

/*i*/

#include <stdio.h>
int main()
{    
    {
        _Defer { fputs("cat says", stdout); }

        // okay: no constraint violation
        goto target;
    }

target:

    fputs(" meow", stdout);
    return 1;  // prints "cat says meow" to standard output
}

```

<button onclick="Try(this)">try</button>


```c
/*j*/

#include <stdio.h>
int main()
{    
    _Defer {
        goto target;  // constraint violation
        fputs(" meow", stdout);
    }

target:

    fputs("cat says", stdout);
    return 1;

}
```
<button onclick="Try(this)">try</button>


```c

/*k*/

#include <stdio.h>
int main()
{    
    _Defer {
        return 5;  // constraint violation
        fputs(" meow", stdout);
    }

    fputs("cat says", stdout);
    return 1;
}
```
<button onclick="Try(this)">try</button>



```c
/*l*/
#include <stdio.h>
int main()
{    
    _Defer 
    {
        target:
        fputs(" meow", stdout);
    }
    goto target;  // constraint violation

    fputs("cat says", stdout);
    return 1;
}
```

<button onclick="Try(this)">try</button>


```c
/*m*/

#include <stdio.h>
int main()
{   
    goto target;  // okay: no constraint violation

    {
        target:
        _Defer { fputs("cat says", stdout); }
    }

    fputs(" meow", stdout);

    return 1;  // prints "cat says meow" to standard output
}
```

<button onclick="Try(this)">try</button>


```c
/*n*/

#include <stdio.h>
int main()
{    
    goto target;  // constraint violation!!

    {
       _Defer {fputs(" meow", stdout); }
       target:
    }

    fputs("cat says", stdout);
    return 1;

}
```
<button onclick="Try(this)">try</button>

```c
/*o*/

#include <stdio.h>
int main()
{   
    {
        _Defer fputs("cat says", stdout);
        goto target;
    }

target:;

    fputs(" meow", stdout);
    return 1;  // prints "cat says meow"

}
```

<button onclick="Try(this)">try</button>

```c
/*p*/
#include <stdio.h>
int main()
{    
    {
        goto target;
        _Defer fputs(" meow", stdout);
    }

target:;

    fputs("cat says", stdout);
    return 1;  // prints "cat says" 
}
```

<button onclick="Try(this)">try</button>

```c
/*q*/

#include <stdio.h>
int main()
{    
    {
        _Defer { fputs(" meow", stdout); }
        target:
    }

    goto target;  // constraint violation !!

    fputs("cat says", stdout);
    return 1;

}
```

<button onclick="Try(this)">try</button>

```c
/*r*/

#include <stdio.h>
int main()
{   
    {
        target:
        _Defer { fputs("cat says", stdout); }
    }

    goto target;  // ok

    fputs(" meow\n", stdout);

    return 1;  // prints "cat says" repeatedly
}
```

<button onclick="Try(this)">try</button>

```c
/*s*/
#include <stdio.h>
int main()
{    
   {
       target:
        _Defer { fputs("cat says", stdout); }
       goto target;  // ok
   }
   
   // never reached
   
   fputs(" meow", stdout);
   
   return 1;  // prints "cat says" repeatedly
}
```

<button onclick="Try(this)">try</button>

```c
/*t*/

#include <stdio.h>
int main()
{   
    int count = 0;
    {
        target:

        _Defer { fputs("cat says ", stdout); }

        ++count;
        if (count <= 2) {
            goto target;  // ok
        }
    }

    fputs("meow", stdout);

    return 1;  // prints "cat says cat says cat says meow"
}
```

<button onclick="Try(this)">try</button>

```c
/*u*/
#include <stdio.h>
int main()
{   
    int count = 0;
    
    {
        _Defer { fputs("cat says", stdout); }

        target:
        
        if (count < 5) {
            ++count;
            goto target;  // ok
        }
    }

    fputs(" meow", stdout);
    return 1;  // prints "cat says meow"
}
```

<button onclick="Try(this)">try</button>


```c
/*v*/
#include <stdio.h>
int main()
{   
    int count = 0;
 
    target:

    if (count >= 2) {
        fputs("meow", stdout);
        return 1;  // prints "cat says cat says meow "
    }

    _Defer { fputs("cat says ", stdout); }

    count++;
    goto target;

    return 0;  // never reached
}
```

<button onclick="Try(this)">try</button>

```c
#include <stdio.h>

/*
   13 EXAMPLE 2 All the expressions and statements of an 
   enclosing block are  evaluated before executing defer 
   statements, including any conversions. After all defer 
   statements are executed, the block is then exited.
*/

int main()
{   
    int r = 4;
    int* p = &r;
    _Defer { *p = 5; }
    return *p;  // return 4;

}
```

<button onclick="Try(this)">try</button>


```c

#include <stddef.h>
#include <stdlib.h>

int use_buffer(size_t n, void* buf) 
{
    /* ... */
    return 0;
}

int main()
{
    const int size = 20;
    void* buf = malloc(size);
    _Defer { free(buf); }
    // buffer is not freed until AFTER use_buffer returns
    return use_buffer(size, buf);
}

```

<button onclick="Try(this)">try</button>

```c

/*
  14 EXAMPLE 3 It is not defined if defer statements execute 
  their deferred blocks if the exiting / non- returning 
  functions detailed previously are called.
*/

#include <stdlib.h>

int f() 
{
    void* p = malloc(1);

    if (p == NULL) {
        return 0;
    }

    _Defer free(p);

    exit(1);  // "p" may be leaked
    return 1;
}

```

<button onclick="Try(this)">try</button>

```c


/*
 15 EXAMPLE 4 Defer statements, when execution reaches them, 
 are tied to the scope of the defer statement within their 
 enclosing block, even if it is a secondary block without 
 braces.
*/

#include <stdio.h>
#include <stdlib.h>

int main() 
{
    {
        _Defer { fputs(" meow", stdout); }
        if (true) _Defer fputs("cat", stdout);
        fputs(" says", stdout);
    }
    // "cat says meow" is printed to standard output
    exit(0);
}

```

<button onclick="Try(this)">try</button>


```c
/*
  16 This applies to any enclosing block, even for loops 
  without braces around its body.
*/

#include <stdio.h>
#include <stdlib.h>
int main() {
    const char* arr[] = {"cat", "kitty", "ferocious little baby"};
    _Defer { fputs(" meow", stdout); }
    for (unsigned int i = 0; i < 3; ++i) _Defer printf("my %s,\n", arr[i]);
    fputs("says", stdout);
    // "my cat,
    // my kitty,
    // my ferocious little baby,
    // says meow"
    // is printed to standard output
    return 0;
}
```

<button onclick="Try(this)">try</button>

```c

/*
 17 EXAMPLE 5 Defer statements execute their deferred blocks 
 in reverse order of the appearance of the defer statements, 
 and nested defer statements execute their deferred blocks 
 in reverse order but at the end of the deferred block they 
 were invoked within. The following program:
*/

int main() {
    int r = 0;
    {
        _Defer {
            _Defer r *= 4;
            r *= 2;
            _Defer { r += 3; }
        }
        _Defer r += 1;
    }
    return r;  // return 20;
}

```
<button onclick="Try(this)">try</button>

```c

/*
   18 EXAMPLE 6 Defer statements can be executed within a 
   switch, but a switch cannot be used to jump into the scope 
   of a defer statement.
*/

#include <stdlib.h>
int main() 
{
    void* p = malloc(1);

    switch (1) {
        _Defer free(p);  // constraint violation
        default:
            _Defer free(p);
            break;
    }
    return 0;
}


```

<button onclick="Try(this)">try</button>

```c

/*
  19 EXAMPLE 7 Defer statements can not be exited by means 
  of break or continue
*/

int main() {
    switch (1) {
        default:
            _Defer {
                break;  // constraint violation
            }
    }
    for (;;) {
        _Defer {
            break;  // constraint violation
        }
    }
    for (;;) {
        _Defer {
            continue;  // constraint violation
        }
    }
    return 0;
}

```

<button onclick="Try(this)">try</button>

```c

/*
 20 EXAMPLE 8 Defer statements that are not reached are 
 not executed.
*/
 
#include <stdlib.h>
int main() {
    void* p = malloc(1);
    return 0;
    _Defer free(p);  // not executed, p is leaked
}


```

<button onclick="Try(this)">try</button>

```c

/*
  21 EXAMPLE 9 Defer statements can contain other 
  compound statements.
*/

typedef struct meow* handle;
extern int purr(handle* h);
extern void un_purr(handle h);

int main()
{
    handle h;
    int err = purr(&h);
    _Defer if (!err) un_purr(h);
    return 0;
}


```

<button onclick="Try(this)">try</button>


###  C2Y if declarations, v4

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3388.htm


```c
#include <stdio.h>
int main()
{
   int size = 10;
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
```
<button onclick="Try(this)">try</button>


### C2Y typename on _Generic

This feature was created in Cake and now it is part of C2Y!

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3260.pdf

```c
 int main()
{
    const int * const p;
    static_assert(_Generic(p, const int *: 1));

    /*extension*/
    static_assert(_Generic(int, int : 1));
    static_assert(_Generic(typeof(p), const int * const: 1));
}
```
<button onclick="Try(this)">try</button>

### C2Y \_\_COUNTER\_\_

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3457.htm

```c

/*
 Compile with -E
*/

#define X(Z) Z Z
X(__COUNTER__) // 0 0
X(__COUNTER__) // 1 1

```
<button onclick="Try(this)">try</button>

### C2Y Local functions
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3678.pdf

```c
int main()
{
   static int dup(int a) { return a * 2; }
   return dup(1);
}
```
<button onclick="Try(this)">try</button>

### C2Y Function Literals
https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3679.pdf

```c
#include <stdio.h>
int main()
{
  printf("%d", (int (void)){
    return 1;
  }());
}

```
<button onclick="Try(this)">try</button>

### C2Y Statement expressions

https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3643.htm

```c

#include <stdio.h> 

#define maxint(a,b) \
  ({int _a = (a), _b = (b); _a > _b ? _a : _b; })

int main()
{
  printf("%d", maxint(1, 2));
}

```

<button onclick="Try(this)">try</button>

## Cake Extensions

### Pre-defined macros in Cake

```c
 #define __CAKE__           202311L
 #define __STDC_VERSION__   202311L
 
```

### assert built-in

In cake assert is an built-in function.
The reason is because it works as tips for flow analysis.

For instance, in a linked list when `head` is null `tail` is also null,
and `tail->next` always points to null.

Assertion will check these properties in runtime and also make 
the static analysis assume that assert evaluates to true.

```c

void list_push_back(struct list* list,
                    struct item* _Owner p_item)
{
   if (list->head == NULL) {
      list->head = p_item;
   }
   else {
      assert(list->tail != nullptr);
      assert(list->tail->next == nullptr);
      list->tail->next = p_item;
   }
   list->tail = p_item;
}
```
<button onclick="Try(this)">try</button>


However, `assert` is not a "blind override command." In situations like:

```c
    int i = 0;
    assert(i != 0);
```

In situations where static analysis can identify two or more possible states, 
assert works as a state selector, similar to what happens in if statements but without the scope.

```c
    void f(int * _Opt p)
    {
        if (p != NULL) {
           //p is not null here...
        }
    }
    
    void f2(int * _Opt p)
    {
        assert(p != NULL);
        //we assume p is not null here...        
    }
```
<button onclick="Try(this)">try</button>


###  try { throw; } catch {}

```
   try-statement:
      try secondary-block
      try secondary-block catch secondary-block   
```

```
jump-statement:
  throw;
```

try catch is a external block that we can jump off.

try catch is a **LOCAL jump** this is on purpose not a limitation.

catch block is optional.

```c
extern int error;

int main()
{
    try
    {
        for (int i = 0 ; i < 10; i++) 
        {
            for (int j = 0 ; j < 10; j++) 
            {
                /*...*/
                if (error) throw;
                /*...*/
            }
        }
    }
    catch
    {
    }
}
```
<button onclick="Try(this)">try</button>

### \#pragma dir  

```c 
#pragma dir "C:/Program Files (x86)/Windows Kits/10//include/10.0.22000.0/cppwinrt"
```  

Add the path to the list of directory paths used to seach include files.


### offsetof

In cake offset (https://en.cppreference.com/w/cpp/types/offsetof.html) is an operator

```c
#include <stdio.h>
 
struct S
{
    char   m0;
    double m1;
    short  m2;
    char   m3;
};
 
int main()
{
    printf("offset of char   m0 = %zu", offsetof(struct S, m0));
    printf("offset of char   m0 = %zu", offsetof(struct S, m1));
    printf("offset of char   m0 = %zu", offsetof(struct S, m2));
    printf("offset of char   m0 = %zu", offsetof(struct S, m3));
}
```

<button onclick="Try(this)">try</button>

### Type traits

We have some compile time functions to infer properties of types.

```c

_is_char()
The three types char, signed char, and unsigned char are collectively called the character types.

_is_pointer
Pointer to object or function

_is_array
Array type

_is_function
A function type describes a function with specified return type. 

_is_floating_point
float, double, and long double return true

_is_integral
The standard signed integer types and standard unsigned integer types are collectively called the
standard integer types;

_is_arithmetic
Integer and floating types are collectively called arithmetic types. 

_is_scalar
Arithmetic types, pointer types, and the nullptr_t type are collectively called scalar types

```

Note: Type traits that can be easily created with \_Generic will be removed.
_
### Extension - Object lifetime checks

See [ownership](ownership.html)

```
_Owner
_Opt
_View
```

## GCC extensions


  * \_\_builtin\_va\_list
  * \_\_builtin\_c23\_va_start
  * \_\_builtin\_va\_start
  * \_\_builtin\_va\_end
  * \_\_builtin\_va\_arg
  * \_\_builtin\_va\_copy
  * \_\_builtin\_offsetof (same as cake offsetof)
  * \_\_attribute\_\_
  * \_\_typeof\_\_ alias same as typeof

Other builtins are declared at `\src\include\x86_x64_gcc_builtins.h`

Pre-defined macros for GCC compatibility
https://gcc.gnu.org/onlinedocs/cpp/Predefined-Macros.html

See `\src\include\x86_x64_gcc_builtins.h`

## MSVC extensions

 * \_\_ptr32, \_\_ptr64
 * \_\_int8 ... \_\_int64
 * \_\_declspec
 * \_\_cdecl
 * \_\_fastcall, \_\_stdcall
 * \_\_forceinline alias for inline in cake
 * \_\_pragma
 * \_\_unaligned 


Pre-defined macros for MSVC compatibility
https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170#standard-predefined-macros

See `\src\include\x86_msvc_macros.h` and `\src\include\x64_msvc_macros.h`
