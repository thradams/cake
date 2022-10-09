
## Command line


```
cake [options] source1.c source2.c ...

Options
  -I                  Adds a directory to the list of directories searched for include files.
                      (On windows, if you run cake at the visual studio command prompt cake 
                      uses the same include files used by msvc.)

  -D                  Defines a preprocessing symbol for a source file.

  -E                  Copies preprocessor output to standard output.

  -r                  Remove all comments from the ouput file.

  -rm                 Outputs preprocessed code after compilation.

  -target=standard    Output target C standard (c89, c99, c11, c2x, cxx).
                      C99 is the default and C89 (ANSI C) is the minimum target.

  -std=standard       Assume that the input sources are for standard (c89, c99, c11, c2x, cxx).
                      (not implented yet, input is considered C23)                    

  -n                  Check naming conventions (it is hardcoded for its own naming convention)

  -fi                 Format input (format before language convertion)

  -fo                 Format output (format after language convertion, result parsed again)

  -default_nodiscard  Makes [[nodiscard]] default implicity.
```

The ouput dir is **./out**

## C99 Transformations

###  C99 restrict pointers

```c
void f(const char* restrict s);
```

Becomes in C89

```c
void f(const char* /*restrict*/ s);
```
> The intended use of the restrict qualifier 
> is to promote optimization, removing it will not 
> change the observable behavior.

###  C99 Variable-length array (VLA) 


The idea is not implement variable length arrays with automatic 
storage duration. (\_\_STDC\_NO\_VLA\_\_ 1). 

But there are other uses of VLA.


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

Becomes C89 (not implemented)

```c
#include <stdlib.h>
#include <stdio.h>

int main() {
	int n = 2;
	int m = 3;
    
    /*these variables are created to store the dynamic size*/
    const int vla_1_n = n;
	const int vla_1_m = m;
    
	int (*p)[n][m] = malloc((vla_1_n*vla_1_m)*sizeof(int));

	printf("%zu\n", (vla_1_n*vla_1_m)*sizeof(int));

	free(p);
}

```


### C99 Flexible array members

```c
struct s {
    int n;
    double d[]; 
};
```

Becomes (not implemented)

```c
struct s {
    int n;
    double d[]; //?
};
```



### C99 static and type qualifiers in parameter array declarators

```c
void F1(int a[static 5]){}
```

Becomes in C89 (not implemented yet)

```c
void F1(int a[/*static*/ 5]){}
```

## Complex and imaginary support
TODO

### C99 The long long int type and library functions
TODO ?

### C99 Universal character names (\u and \U)
TODO

###  C99 Hexadecimal floating constants

```c
double d = 0x1p+1;
```
Becomes in C89

```c
double d = 2.000000;
```

>Cake converts the hexadecimal floating to decimal
>floating point using strtod then snprintf.
>That means this convertion is not precise.
>


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
Becomes in C89 (not implemented yet)

```c
struct s {
  int i;
};
int f(void) {
  struct s * p = 0, * q;
  int j = 0;
  again:
    struct s compound_literal_1 = { j++ };
    q = p, p = & compound_literal_1;
  if (j < 2) goto again;
  return p == q && q -> i == 1;
}
```

### C99 designated initializers

```c
 int main()
 {
  int a[6] = {[4] = 29, [2] = 15 };

  struct point { int x, y; };

  struct point p = { .y = 2, .x = 3 }
 }

```

Becomes C89 (not implemented yet)

```c
int main()
{
  int a[6] = { 0, 0, 15, 0, 29, 0 };
  struct point { int x, y; };

  struct point p = { 3, 2 }

}

```


### C99 line comments

When compiling to C89 line comments are converted to comments.

```c
//line comments
```

Becomes in C89

```c
/*line comments*/
```


### C99 mixed declarations and code
TODO

```c

//-std=c90 -pedantic

int main()
{
    int i;
    /* ... */
    i++;
    int j = i + 2; //warning: ISO C90 forbids mixed declarations and code
}
```

Expected in C89 (not implemented yet)

```c
int main()
{
    int i;
    int j;
    /* ... */
    i++;
    j = i + 2;
}

```


### C99 inline functions

TODO

### _Pragma preprocessing operator
TODO 

### \_\_func\_\_ predefined identifier
TODO

###  init-statement in for loops 

```c
int main()
{
    for (int i = 0; i < 10; i++)
    {
    }
}
```

Becomes in C89 (not implemented yet)

```c
int main()
{
    {
       int i = 0;
       for (;i < 10; i++)
       {
       }
    }
}
```

###  C99 Variadic macros

Yes. We need to expand the macro when comping to C89.

This is covered by # macro expand.

Sample:

```c

#include <stdio.h>

#define debug(...) fprintf(stderr, __VA_ARGS__)
#pragma expand debug

int main()
{
  int x = 1;
  debug("X = %d\n", 1);
}

```
Becomes

```c
#include <stdio.h>

#define debug(...) fprintf(stderr, __VA_ARGS__)
#pragma expand debug

int main()
{
  int x = 1;
  fprintf(stderr, "X = %d\n", 1);
}

```

>I am considering to mark the debug macro to be expanded automatically
>if \_\_VA\_ARGS\_\_ is used. Then pragma expand will not be necessary.

###  Trailing comma in enumerator-list

TODO. We could remove this extra comma. (low prioriry)

###  C99 \_Bool
When compiling to C89 _Bool is replaced by int.

```c
//line comments
int main(void)
{
    _Bool b = 1;
    return 0;
}
```

Becomes in C89

```c
/*line comments*/
int main(void)
{
    int b = 1;
    return 0;
}
```

## C99 Improvements of braced-init-list for array, struct and union types 

```c
```

```c
```

## C11 Transformations

###  C11 \_Static\_assert

When compiling to versions C89, C99 \_Static\_Assert is removed

### C11 Anonymous structures and unions
TODO

### C11 \_Noreturn


```c
_Noreturn void f () {
  abort(); // ok
}
```

Expected in C99, C89 (not implemented yet)

```c
/*[[noreturn]]*/ void f () {
  abort(); // ok
}
```

>Note: 
>C23 attribute [[noreturn]] provides similar semantics. The _Noreturn function specifier is 
> an obsolescent feature



###  C11 Thread_local/Atomic
Parsed but not transformed.

###  C11 type-generic expressions (\_Generic)

When compiling to C99, C89 we keep the expression that matches the type.

For instance:

The expression that matches the argument 1.0 is **cbrtl**.

The result of \_Generic in C99 will be cbrtl. Because this is inside
a macro we need to tell the transpiler to expand that macro using 
pragma expand.


```c
#include <math.h>

#define cbrt(X) _Generic((X), \
                  double: cbrtl, \
                  float: cbrtf ,\
                  default: cbrtl  \
              )(X)

#pragma expand cbrt


int main(void)
{
    cbrt(1.0);
}

```

Becomes in C99, C89

```c

#include <math.h>

#define cbrt(X) _Generic((X), \
                  double: cbrtl, \
                  float: cbrtf ,\
                  default: cbrtl  \
              )(X)

#pragma expand cbrt


int main(void)
{
     cbrtl(1.0);
}

```

###  C11 u8"literals"

u8 literals are converted to escape sequecences.

```c
char * s1 = u8"maçã";
char * s2 = u8"maca";

```

Becomes in C99, C89

```c
char * s1 = "ma\xc3\xa7\xc3\xa3";
char * s2 = "maca";
```


## C23 Transformations

###  C23 \_Decimal32, \_Decimal64, and \_Decimal128
Not implemented (maybe parsed?)

### C23 static\_assert / single-argument static_assert
In C23 static\_assert is a keyword and the text message is optional.

Whe comping to C11, static\_assert is replaced by \_Static\_assert
If the static\_assert has only one argument the text becomes "error".


```c
int main()
{    
    static_assert(1 == 1, "error message");
    static_assert(1 == 1);
}

```
Becomes in C11

```c
int main()
{    
    _Static_assert(1 == 1, "error message");
    _Static_assert(1 == 1, "error");
}
```
In C99, C89 it is replaced by one space;

###  C23 u8 char literal
not implemented yet.

### C23 Digit separators

```c
int main()
{
    int a = 1000'00;
}
```
Becomes in C11, C99, C89

```c
int main()
{
    int a = 100000;
}x  
```

>This transformation uses only tokens. So even preprocessor and inactive
>code is transformed.

### C23 Binary literals 

```c
#define X  0b1010

int main()
{
    int a = X;
    int b = 0B1010;
}

```
Becomes in C11, C99, C89

```c
#define X  0xa

int main()
{
    int a = X;
    int b = 0xa;
}

```

>This transformation uses only tokens. So even preprocessor and inactive
>code is transformed.


### C23 nullptr

```c
#include <stdlib.h>

int main()
{
  void * p = nullptr;
  void * p2 = NULL;
}

```

Becomes in C11, C99 C89

```
int main()
{
  void * p = ((void*)0);
}
```


### C23 bool true false

When compiling to C89 bool is replaced by int true by 1 and false 0. 
(I am considering adding an extra header file in C89 with bool definition.)

When compiling to C99 and C11 bool is replaced with **_Bool**, true is replaced with `((_Bool)1)` 
and false with **(_Bool)0)**


###  C23 {} empty initializer

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

Becomes in C11, C99, C89

```c

int main()
{
    struct X {
        int i;
    } x = {0};

    x = (struct X) {0};

    struct Y
    {
        struct X x;
    } y = { {0} };
}

```

> Note: Cake code is 100% equivalent because it does not make padding bit zero.

###  C23 auto


```c
static auto a = 3.5;
auto p = &a;

double A[3] = { 0 };
auto pA = A;
auto qA = &A;

```
Expected result (not implemented yet)

```c
static double a = 3.5;
double * p = &a;

double A[3] = { 0 };
double* pA = A; //TODO
auto qA = &A; //TODO?
```

###  C23 typeof / typeof_unqual


```c

#define SWAP(a, b) \
  do {\
    typeof(a) temp = a; a = b; b = temp; \
  } while (0)

#pragma expand SWAP

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
   typeof(void (int)) * pf = NULL;
}

```
Becomes in C11, C99, C89

```c


#define SWAP(a, b) \
  do {\
    typeof(a) temp = a; a = b; b = temp; \
  } while (0)

#pragma expand SWAP

int main()
{
    /*simple case*/
    int a = 1;
    int  b = 1;

    /*pay attention to the pointer*/
    int  *p1,  *p2;

    /*let's expand this macro and see inside*/
     do {int temp = a; a = b; b = temp; } while (0);

    /*for anonymous structs we insert a tag*/
    struct _anonymous_struct_0 { int i; } x;
    struct _anonymous_struct_0  x2;
    struct _anonymous_struct_0  x3;

   /*Things get a little more complicated*/
   int *array[2];
   int  *a1[2],  *a2[2];
   
   int  *(a3[3])[2];
   int  *(*a4[4])[2];

   /*abstract declarator*/
   int k = sizeof(int*[2]);


   /*new way to declare pointer to functions?*/
   void  (*pf)(int) = ((void*)0);
}


```

###  C23 constexpr
Not implemented yet (maybe parsed?)


###  C23 enuns with type

```c
enum X : short {
  A
};

int main() {
   enum X x = A;   
}
```

Becomes? in C11, C99, C89 (not implemented yet)

```c
enum X {
  A
};

int main() {
   short x = ((short)A);   
}
```


###  C23 Attributes

Attributes are being parsed and removed in some places. 
More work is necessary to implement the checks in C23.

Convertion to C11, C99, C89 will just remove the attributes.


###  C23 \_\_has\_attribute

It is implemented in C23.

Convertion to C11, C99 C89 not defined yet.


###  C23 \_\_has\_include

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

Becomes in C11, C99, C89.

```c
Not defined yet
```


###  C23 #warning
When compiling to versions < 23 it is commented out.

The compiler also ouputs the message on stderr.

```c
int main()
{
  #warning my warning message  
}
```
Becomes

```c
int main()
{
  //#warning my warning message  
}
```


###  C23 #embed

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
Becomes in C11, C99, C89

```c


#include <stdio.h>

int main()
{
  static const char file_txt[] = {
    35,112,114,/*lot more here ...*/ 13,10
   ,0
  };

  printf("%s\n", file_txt);

}

```

I am considering add an option to generate a file with a suffix
like "embed_stdio.h" then the equivalent code will be:

Becomes in C11, C99, C89

```c
#include <stdio.h>

int main()
{
  static const char file_txt[] = {
   #include "embed_stdio.h"
   ,0
  };

  printf("%s\n", file_txt);

}

```




###  C23 \_\_VA_OPT\_\_

Requires #pragma expand. (TODO make the expansion automatic)

```c

#define F(...) f(0 __VA_OPT__(,) __VA_ARGS__)
#define G(X, ...) f(0, X __VA_OPT__(,) __VA_ARGS__)
#define SDEF(sname, ...) S sname __VA_OPT__(= { __VA_ARGS__ })
#define EMP

/*maybe this could be automatic if <C23*/
#pragma expand F
#pragma expand G
#pragma expand SDEF
#pragma expand EMP

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


Becomes in C11, C99, C89

```c

#define F(...) f(0 __VA_OPT__(,) __VA_ARGS__)
#define G(X, ...) f(0, X __VA_OPT__(,) __VA_ARGS__)
#define SDEF(sname, ...) S sname __VA_OPT__(= { __VA_ARGS__ })
#define EMP

/*maybe this could be automatic if <C23*/
#pragma expand F
#pragma expand G
#pragma expand SDEF
#pragma expand EMP

void f(int i, ...) {}


int main()
{
  int a = 1;
  int b = 2;
  int c = 3;
  
   f(0, a, b, c);
   f(0 );
   f(0);
   f(0, a, b, c);
   f(0, a );
   f(0, a );

}
```


###  C23 BitInt(N))

Not implemented

###  C23 constexpr

Parsed but not implemented.

```c

 constexpr int K = 47;
 enum {
  A = K, // valid, constant initialization
 };
 constexpr int L = K; // valid, constexpr initialization
 static int b = K + 1; // valid, static initialization
 int array[K]; // not a VL

```


### Compound Literals with storage specifier

```c
void F(int *p){}

int main()
{
   F((static int []){1, 2, 3, 0})
}
```

becomes (not implemented yet)

```c
void F(int *p){}

int main()
{
    static int _compound_1[] = {1, 2, 3, 0};
    F(_compound_1);
x   }
```

###  C23 #elifdef #elifndef

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

Becomes C11, C99, C89

```c
#define Y

#ifdef X
#define VERSION 1
#elif defined   Y
#define VERSION 2
#else
#define VERSION 3
#endif

```

## Extensions (Not in C23)

###  Extension - try catch throw


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

try catch is a LOCAL jump only and this is on purpose not a limitation.


catch block is optional.

```c
try
{
   for (int i = 0 ; i < 10; i++) {
      for (int j = 0 ; j < 10; j++) {
        ... 
        if (error) throw;
        ...
      }
   }
}
catch
{
}
```

###  Extension - defer

*defer* will call the defer statement before the block exit at inverse orden of declaration.

```
     defer-statement:
        defer secondary-block
```

For instance:

```c
#include <stdio.h>

int main() {
  do {
     FILE* f = fopen("in.txt", "r");
     if (f == NULL) break;
     defer fclose(f);

     FILE* f2 = fopen("out.txt", "w");
     if (f2 == NULL) break;
     defer fclose(f2);
     //...    
  }
  while(0);
}
```

Becomes in C23, C11, C99, C89

```c
#include <stdio.h>

int main() {
  do {
     FILE* f = fopen("in.txt", "r");
     if (f == ((void*)0)) break;

     FILE* f2 = fopen("out.txt", "w");
     if (f2 == ((void*)0)) {  fclose(f); break;}
     
     fclose(f2); fclose(f);
   }
  while(0);
}
```
I guess everthing is working including **goto** jumps.

###  Extension - if with initializer

No idea why C++ 17 if with initializer was not proposed for C23!
But in cake it is implemented.

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
}
```
Becomes

```c
#include <stdio.h>

int main()
{
   int size = 10;
   {FILE* f = fopen("file.txt", "r");if ( f)
   {
     /*...*/
     fclose(f);
   }}
}
```
An extension if + initializer + defer expression was considered but not implemented yet.

###  Extension Literal function - lambdas

Lambdas without capture where implemented using a syntax similar of 
compound literal for function pointer.

Lambdas are the most complex code transformation so far because sometimes function scope 
types needs to be transformed to file scope. This is important because manual lambda capture
is something we want to use in function scope.

For instance:

```c
extern char* strdup(const char* s);
void create_app(const char* appname)
{
  struct capture {
     char * name;
  } capture = { .name = strdup(appname) };

  (void (void* p)) {
    struct capture* capture = p;    
  }(&capture); 
}
```
Because struct capture was in function scope and the lambda function will be created
at file scope the type **struct capture** had to be moved from function scope to file scope.

```c
extern char* strdup(const char* s);

struct _capture0 {
     char * name;
  };
  
void _lit_func_0(void *p) {
    struct _capture0* capture = p;    
  }

void create_app(const char* appname)
{
  struct _capture0  capture = { .name = strdup(appname) };
  _lit_func_0(&capture);  
}
```

###  Extension typeid

syntax:

```c
  typeid (expression)
  typeid (type-name)
```

typeid returns a **type object** that can be used with == and !=.
If any left or right side of the ==/!= contains a **type object** then
the evaluation is based on type comparison not value.

Sample:

```c
static_assert(1 == typeid(int));
static_assert(typeid(1) == typeid(int));
```

###  Extension Repeat


```
iteration-statement:
        ...
        "repeat" secondary-block  (extension)
```
  repeat statement
```

Repeat merely executes the given statement forever unless a break statement is encountered, or a goto passes control to a statement outside the loop.

```c
  repeat {
       if (condition) 
         break;
  }
```

Repeat is equivalent of for(;;) 


### Extension #pragma expand

pragma expand tells the back end to not hide macro expansions.

Sample: 


```c

#define SWAP(a, b) \
    do { \
      typeof(a) temp = a; a = b; b = temp; \
    } while(0)

#pragma expand SWAP

int main()
{
   int a = 1;
   typeof(a) b = 2;
   SWAP(a, b);
   return 1;
}
```

Becomes

```c
#define SWAP(a, b) \
    do { \
      typeof(a) temp = a; a = b; b = temp; \
    } while(0)

#pragma expand SWAP

int main()
{
   int a = 1;
   int b = 2;
    do {int temp = a; a = b; b = temp; } while(0);
   return 1;
}

```

###  Extension _Hashof

_Hashof is a compile time function that returns a hash of the parsing tokens
of some struct enum etc.


```c

struct X {
    int a[10];
  
    /*uncomment the next line*/
    //char * text;
};

void x_destroy(struct X* p);

int main()
{
    struct X x = {};
    x_destroy(&x);
}

void x_destroy(struct X* p)
{
    /*In a far away galaxy*/
    static_assert(_Hashof(p) == 283780300, "check this function x_destroy");
}

```

###  Extension - declarator static analisys flags

Motivation:

When we review code we follow some "imaginary operations" in code.
For instance:

```c
int *p = malloc(sizeof(int)*10);
free(p);
p[0] = 1;
```

When we free(p) we know that p is invalid and should not be used anymore.

#### How we could take notes that p is invalid?
#### How compiler could check our notes?

This is the motivation for *declarator static analisys* flags.

### Mechanics.

We can set/remove and checks declarator flags.

For instance:

```c
int main()
{
    int *p = malloc(sizeof(int)*10);
    _add_attr(p, MUST_FREE);

    free(p);
    _del_attr(p, MUST_FREE);

    static_assert(!__has_attr(MUST_FREE));
}
```

When we set/remove or check declarator flags on arguments
the operation is delayed until the function instantiation.

For instance:

```c
void my_free(void *arg1)
{
    static_assert(__has_attr(arg1, MUST_FREE));
    _del_attr(arg1, MUST_FREE);
}
```

We cannot determine if arg1 has the flag MUST\_FREE without
looking at caller. So static_assert is just ignored at first
pass.

But when we call my_free:

```c
int main()
{
    int *p = malloc(sizeof(int)*10);
    _add_attr(p, MUST_FREE);

    my_free(p);

    static_assert(!__has_attr(MUST_FREE));
}
```

Then the flags of p are plugged into arg1 by "reference" and the function
my_free is analised again. set/remove and check are now using the flags of p.

We can also add flags to the receiver declarator using "return"

```c
void* my_malloc(int size)
{    
    _add_attr(return, MUST_FREE);
}
```

So we can write

```c
int main()
{
    int *p = my_malloc(sizeof(int)*10);
    static_assert(__has_attr(MUST_FREE));

    my_free(p);
    static_assert(!__has_attr(MUST_FREE));
}
```

Cake have some built in flags on header annotations.h. 

It also  have some built in checks.

When the declarators go out of scope we check if MUST\_FREE flag
is cleared.

There is also a MUST\_DESTROY flag that is used when other function
needs to be called. For instance, x_destroy for struct x.


