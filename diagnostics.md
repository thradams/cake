## Warnings

Warnings can be enabled with `-w<number>` (e.g. `-w2`) and disabled with
`-wd<number>` (e.g. `-wd2`). `-wall` enables all of them.

All warning are configurable. Cake has errors, warnings and notes.
We can make a warning to be a note or error using pragma.

```c
#pragma CAKE diagnostic push
#pragma CAKE diagnostic error 4
    

    /*
      4 is not reported as error
      code..
    
    */

#pragma CAKE diagnostic pop

```


### 1 \#warning directive (preprocessor)
<!-- runnable -->

```c
#warning message
```

### 2 Unused variable

<!-- runnable -->

```c
// -w02
void f()
{
    int i;    
}
```

### 3 Deprecated

<!-- runnable -->

```c
[[deprecated]] void f2() {}

int main(void)
{
    f2();
}
```


### 4 Enum conversion issue
<!-- runnable -->

```c
enum E1 { A };
enum E2 { B };

int main(void)
{
    enum E1 e = A;
    return e == B; //warning 4: comparing different enums (enum E1, enum E2)
}
```

### 5 token sliced


### 6 Unused function parameter (disabled by default)

<!-- runnable -->

```c
// -w06
void f(int i) {}
```
### 7 Declarator hides previous declaration

<!-- runnable -->

```c
void f() {
    int i;
    {
      int i;
    }
}
```
### 8 typeof used on array parameter

<!-- runnable -->

```c
void f(int a[])
{
    typeof(a) p;
}
```
### 9 ignoring the result of _Owner type 

<!-- runnable -->

```c
#pragma safety enable
int *_Owner f();
int main()
{
  f();  //warning C0009: ignoring the result of _Owner type 
}
```

### 10 Expression result not used

<!-- runnable -->

```c
int main()
{  
  int a = 0;
  a++;
  
  a; //warning C0010: expression result not used
  1; //warning C0010: expression result not used
 
}
```


### 11 Style issue (disabled by default)
<!-- runnable -->

```c
// -w11
int main(void) { //warning 11: not following correct brace style {
    return 0;
}
```

### 12  multi-line comment

<!-- runnable -->

```c
int main()
{  
  // \
  a++; 
}
```


### 13 Line slicing occurred
<!-- runnable -->

```c
int a \
   = 1; // warning C0013: unnecessary line-slicing
}
```


### 14 String was sliced (not emitted)
Reserved (`W_STRING_SLICED`). Not emitted by the current compiler.

### 15 Discarded qualifiers

<!-- runnable -->

```c
char* f(){
    static const char s[] = "a";
    return s; //warning C0015:  discarding const
}
```

Both operands of `?:` contribute: per C11 6.5.15p6 the result of a pointer
conditional is qualified with the qualifiers of *both* referenced types, so a
const operand makes the whole result const.

**Not reported:** an explicit cast that removes the qualifier
(`g((struct X*)p)`). That is legal C -- the cast is the author taking
responsibility -- so no diagnostic is possible, and none of the const checks
can see through it.

<!-- runnable -->

```c
struct X { int i; };
struct X* g_ptr;

void f(const struct X* p, int c)
{
    struct X* q = c ? p : g_ptr; //warning C0015: discarding const qualifier
    (void)q;
}
```


### 16 (unused)


### 17 Uninitialized variable (not emitted)
Reserved (`W_UNINITIALZED`). Not emitted by the current compiler.

### 18 Returning address of local variable (not emitted)
Reserved (`W_RETURN_LOCAL_ADDR`). Not emitted by the current compiler.

### 19 Missing address-of operator (not emitted)
Reserved (`W_MUST_USE_ADDRESSOF`). Not emitted by the current compiler.

### 20 Array indirection issue
<!-- runnable -->

```c
void f(int a[])
{
    *a =1; // warning C0020: array indirection
}
```


### 21 (unused)

### 22 Using object without being owner (not emitted)
Reserved (`W_FLOW_NOT_OWNER`). Not emitted by the current compiler.

### 23 Using temporary owner incorrectly (flow) (not emitted)
Reserved (`W_FLOW_USING_TEMPORARY_OWNER`). Not emitted by the current compiler.

### 24 Move-assignment to non-owner (not emitted)
Reserved (`W_FLOW_MOVE_ASSIGNMENT_OF_NON_OWNER`). Not emitted by the current compiler.

### 25 Assigning non-owner to owner (flow) (not emitted)
Reserved (`W_FLOW_NON_OWNER_TO_OWNER_ASSIGN`). Not emitted by the current compiler. The parse-time check for this case is warning 78.

### 26 Discarding an owner (flow)
<!-- runnable -->

```c

#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);

void f(int * _Opt p){}

int main() {
   f((int*)malloc(1)); //warning C0026: discarding _Owner pointer   
}
```

### 27 (unused)

### 28 Non-null flow violation (not emitted)
Reserved (`W_FLOW_NON_NULL`). Not emitted by the current compiler.

### 29 pointed object was not released (flow)
<!-- runnable -->

```c
#pragma safety enable

struct X {
    char* _Owner text; 
};

void delete_x(struct X *_Owner p) //warning C0029: object pointed by 'p' was not released.
{
}
int main() {}
```

### 30 Uninitialized value (flow)

This warning is issued when a local variable is used before it has 
been initialized. 

<!-- runnable -->

```c
#pragma safety enable

void f(int i)
{
    int j;
    if (i) j =1;
    i = j; //warning C0030: object 'j' can be uninitialized
}
int main() {}

```

<!-- runnable -->

```c

#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

struct X{
    char* _Owner _Opt p;
};
struct X makeX();
void destroyX([[dtor]]struct X * p){
  free(p->p);
}

int main() {
    struct X x = makeX();
    destroyX(&x);
    destroyX(&x); //double free
}

```


### 31 Lifetime has ended (flow)
<!-- runnable -->

```c

#pragma safety enable

struct X { int i; };

int main() {
    struct X * _Opt p = 0;
    {
      struct X x ={};
      p = &x;
    }
    p->i = 0; //warning C0031: operator -> applied to 'p->i': pointed object lifetime has ended (see line ...)
}

```


See [object lifetime](ownership.md)


### 32 Object already moved (flow)
<!-- runnable -->

```c

#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

int main() {
   void * _Owner _Opt p = malloc(1);
   void * _Owner _Opt p2 = p;
   void * _Owner _Opt p3 = p; //warning C0032: object may be already moved
   free(p2);
}

```


### 33 Null dereference (flow)

This warning indicates that your code dereferences a potentially null pointer. 

<!-- runnable -->

```c
#pragma safety enable
int main()
{
  int * _Opt p = 0;
  *p =1; //warning C0033: possible null pointer dereference '*p'
}
```



### 34 (not used flow)

### 35 Nullable converted to non-nullable (flow)
<!-- runnable -->

```c
#pragma safety enable

void f(int *p);
int main()
{
  int * _Opt p = 0;
  f(p); // warning C0035: passing a possible null pointer 'p' to non-nullable pointer parameter
}
```


### 36 Division by zero (flow)
<!-- runnable -->

```c
#pragma safety enable

void f(int i)
{
    if (i > 0)
     i = 0;
    else
     i =1;

    int j = 1/i; //warning C0036: possible division by zero
}
```

### 37 Division by zero (without flow analysis)

<!-- runnable -->

```c
int main()
{
    const int i = 0;
    int j = 1/i; //warning C0037: division by zero
}
```



### 38 (not used)

### 39 Passing null as array
<!-- runnable -->

```c
void f(int a[]){}
int main(){
    f(0); //warning C0039:  passing null as array
}

```



### 40 Incompatible enum types
<!-- runnable -->

```c
enum E1 {A};
enum E2 {B};
int main()
{    
   enum E1 e = B; //warning C0040:  incompatible types
}
```

### 41 Multi-character literal
<!-- runnable -->

```c
int main(void)
{
    int i = u'ab'; //warning 41: Unicode character literals may not contain multiple characters.
    (void)i;
}
```

### 42 Out-of-bounds access
<!-- runnable -->

```c
int main()
{  
  int a[2]= {0};
  a[2] = 1; //warning C0042: index 2 is past the end of the array
}
```

### 43 Assignment to array parameter
<!-- runnable -->

```c
void f(int a[]){
    a = 1; // warning C0043: assignment to array parameter
}
```

### 44 Conditional is constant (currently disabled)

<!-- runnable -->

```c
int main()
{  
  if (1) {
  }
}
```

### 45 Suspicious switch statement

<!-- runnable -->

```c
int main()
{
    bool b =1;
    switch(b){ //warning C0045: switch condition has boolean value
        case 1:break;
    }
}

```


### 46 Unusual null pointer constant
<!-- runnable -->

```c
int main()
{
    int * p = '\0'; //warning C0046: unusual expression/type used as null pointer constant
}
```

### 47 sizeof applied to array argument
<!-- runnable -->

```c
void f(int a[])
{
    sizeof(a); //warning C0047: sizeof applied to array function parameter
}
```

### 48 const object not initialized
<!-- runnable -->

```c
int main()
{  
  const int i;
}
```

### 49 implicit conversion of nullptr constant to 'bool'
<!-- runnable -->

```c
void f(bool b){}
int main(){
    f(nullptr); //warning C0049: implicit conversion of nullptr constant to 'bool'
}
```


### 50 Implicitly unsigned literal
<!-- runnable -->

```c

int main(){
  long long a = 9223372036854775807;
  unsigned long long b = 9223372036854775808; // warning C0050: integer literal is too large to be represented in a signed integer type, interpreting as unsigned
  unsigned long long c = 9223372036854775808ULL;
}

```

### 51 Integer overflow
<!-- runnable -->

```c

int main(){
  long long a = 9223372036854775807 + 2; //warning C0051: integer overflow
}

```

### 52 Invalid array size
<!-- runnable -->

```c
int main()
{
    char a[2] = "1234"; // warning C0052: initializer for array is too long
}
```

### 53 (unused)


### 54 Incompatible types
<!-- runnable -->

```c
void f(double * p);
int main()
{  
    int a;
    f(&a); // warning C0054:  incompatible types
}
```


### 55 Unused label
<!-- runnable -->

```c
int main()
{  
  A: //warning C0055: label 'A' defined but not used
}
```
### 56 Redefining builtin macro

<!-- runnable -->

```c
#define __FILE__ 1 //warning C0056: redefining built-in macro
```


### 57 Unused function
<!-- runnable -->

```c
static void f(){};
int main()
{  
}
```


### 58 Boolean comparison issue
<!-- runnable -->

```c
int main()
{
    bool b = false;
    int i = 2;
    if (b == i) //warning C0058: comparison bool with non bool
    {
    }
}
```

### 59 Expected warning did not occur
<!-- runnable -->

```c
int main()
{
    bool b = false;
    bool i = false;
    if (b == i) //lint 58 warning C0059: warning 'C0058' was not recognized
    {
    }
}
```

### 60 Null pointer constant to non-nullable pointer (not emitted)
Reserved (`W_NULLABLE_TO_NON_NULLABLE`). Not emitted by the current compiler.

### 61 Cast to same type (not emitted)
Reserved (`W_CAST_TO_SAME_TYPE`). Not emitted by the current compiler.

### 62 Too many initializers

<!-- runnable -->

```c
int main()
{  
  int a[2] = {1, 2, 3};
}
```

### 63 Float out of range
<!-- runnable -->

```c
int main() {
  float f = 1e300f; //warning C0063: floating constant exceeds range of float
}
```

### 64 Mixed string literal prefixes
<!-- runnable -->

```c
int main(void)
{
    const char* s = "abc" L"def"; //warning 64: concatenation of string literals with different encoding prefixes
    (void)s;
}
```

### 65 Signed to unsigned (not implemented)

Reserved. The check is not emitted yet.

### 66 Information note (reserved)

Reserved for informational notes attached to another diagnostic. Not emitted yet.

### 67 compile_assert could not be proven (flow)
<!-- runnable -->

```c
#pragma safety enable

int f();

int main() {
    int i = f();
    compile_assert(i == 1); //warning C0067: compile_assert failed: value could be any value, including zero, set at line 5 in "root"
}
```

### 68 Unreachable code (flow)
<!-- runnable -->

```c
#pragma safety enable

int f() {
    return 1;
    return 2; //warning C0068: unreachable code
}
```

### 69 _Clear parameter pointee not zeroed at exit (flow)
<!-- runnable -->

```c
#pragma safety enable

struct outer { int a; int c; };

void clear_outer_forgets_a_member(_Clear struct outer* p)
{
    p->a = 0;
    /* forgot p->c = 0; */
} //warning C0069: _Clear parameter 'p->c' is not zero at exit (see line 3)
```

### 70 Array index out of bounds (flow)
<!-- runnable -->

```c
#pragma safety enable

int main(int cond) {
    int a[2];
    int i;
    if (cond) i = 2; else i = 3;
    a[i] = 1; //warning C0070: array index is past the end of the array (size 2)
}
```

### 71 _Out parameter pointee not initialized at exit (flow)
<!-- runnable -->

```c
#pragma safety enable

struct outer { int a; int c; };

void ctor_outer_forgets_a_member(_Out struct outer* p)
{
    p->a = 1;
    /* forgot p->c = ...; */
} //warning C0071: _Out parameter 'p->c' is possibly not initialized at exit (see line 3)
```

### 72 Borrowed parameter's owner consumed at exit (flow)
<!-- runnable -->

```c
#pragma safety enable

typedef unsigned long size_t;
char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct person { char* _Opt _Owner name; };

void set(struct person* p, char* name)
{
    free(p->name);
    char* _Opt _Owner temp = strdup(name);
    if (temp == 0) return; //warning C0072: parameter 'p->name' was moved/released here (see line ...) but never reassigned -- only a _Dtor parameter may leave the caller's object consumed
    p->name = temp;
}
```

### 76 Logical operation on address of string constant
<!-- runnable -->

```c
int main()
{
    char* pc;
    pc = "Hello";
    if (pc == "Hello") //warning C0076: logical operation on address of string constant
    {
    }
}
```

### 82 Parameter could point to const

The pointed object is never written through, and no pointer into it escapes
somewhere that could write it later, so the parameter can promise more. The
suggestion applies to every pointer spelling -- `const int* p`,
`const int a[]`, `const int (*a)[10]`.
Passing it on to a `const`-qualified parameter keeps the promise; passing it
to a non-`const` one does not.

Assigning to the *pointer* (`p = 0`) does not count -- that is about
`T* const p`, not about the pointee. Neither does writing through a pointer
*member* (`p->buf[0] = 'x'`), which does not modify `*p` and stays legal under
`const struct X*`. Writing an *array* member (`p->arr[0] = 1`) does count.

**Not reported.** The check answers "does anything write through this
parameter", not "would `const` compile", so it stays quiet whenever it loses
track of the pointer:

- the pointer is copied to another variable (`struct X* q = p;`) -- writes
  through `q` are not attributed back to `p`;
- the pointer goes through an explicit cast (`g((struct X*)p)`), or is passed
  to a `...` parameter, or to any destination whose pointee is not `const`;
- the parameter is `_Out`, `_Dtor`, `_Clear` or `_Owner`, all of which write
  the pointee by contract;
- the parameter is never dereferenced at all -- a bare `(void)p` is
  [6 Unused function parameter](#6-unused-function-parameter-disabled-by-default)'s
  subject, not this one;
- the only dereference is inside an unevaluated operand -- `sizeof(*p)`,
  `_Countof`, `_Alignof`, or the unselected arm of a `_Generic` -- which names
  the pointee's type without touching the object.

The one case that can still produce a *wrong* suggestion is a pointer stored
somewhere the assignment checks never visit, such as inside a compound
literal.

<!-- runnable -->

```c
// -w082
struct X { int i; };

int get_i(struct X* p)
{
    return p->i;
}
```

### 83 Parameter set but not used (disabled by default)

The caller already supplied a value, so assigning to the parameter and never
reading it back makes the assignment dead. `x++` and `x += 1` count as a set
only when their result is discarded -- `while (x--)` reads it.

**Not reported:** the same cases as
[84](#84-variable-set-but-not-used-disabled-by-default) below.

<!-- runnable -->

```c
// -w083
int side(void);

void f(int x)
{
    x = side();
}
```

### 84 Variable set but not used (disabled by default)

Assigned at least once and never read. A variable that is only *initialized*
and never read is reported as [2 Unused variable](#2-unused-variable)
instead. Taking the address of the variable, or declaring it `volatile`,
suppresses the warning.

**Not reported.** Only the declarator an lvalue *names* is treated as set, so
writing through a part of it is a use of the whole:

- `a[0] = 1` is a use of `a`, and `s.f = 1` a use of `s`, so neither array nor
  struct is reported even when nothing ever reads it back;
- `*p = 1` is a use of `p` (it reads the pointer), never a set of it;
- `sizeof(n)` counts as a read, so it suppresses the warning;
- `&n` anywhere suppresses it -- the address escaping means a read can happen
  out of sight;
- `volatile` suppresses it: writing a volatile object is the point.

<!-- runnable -->

```c
// -w084
int side(void);

void f(void)
{
    int n = 0;
    n = side();
}
```

### 85 Condition is always true or false (flow) (disabled by default)

The `if` condition is already decided by what flow analysis knows at that
point, on every path reaching it -- so one of the two branches can never
run. A condition whose value differs between paths is not reported.

Off by default: plenty of deliberate code has a decided condition (a guard
kept for clarity, a test on a build-time macro), and inside a `while`/`do`
body the analysis reasons from the state after one iteration, so a condition
on a variable that body itself assigns can read as decided when the first
iteration says otherwise. Enable it with `-w085` when auditing.

<!-- runnable -->

```c
// -w085
#pragma safety enable

int main() {
    int i = 1;
    if (i) //warning C0085: condition is always true
    {
    }
}
```

### 73 Unknown escape sequence
<!-- runnable -->

```c
int main(void)
{
    char c = '\q'; //warning 73: unrecognized character escape sequence '\q'
    (void)c;
}
```

### 74 Constant not exactly representable
<!-- runnable -->

```c
int main(void)
{
    float f = 0.1; //warning 74: constant expression is not exactly representable in type 'float'
    (void)f;
}
```

### 75 Pointer to integer conversion
<!-- runnable -->

```c
int main(void)
{
    int i;
    int* p = &i;
    int n = p; //warning 75: pointer to integer conversion
    (void)n;
}
```

### 77 Static function declared but not defined
<!-- runnable -->

```c
static void f(void); //warning 77: static function 'f' declared but not defined

int main(void)
{
    f();
}
```

### 78 Assigning non-owner to owner
<!-- runnable -->

```c
#pragma ownership enable
void free(void* _Owner _Opt p);

int main(void)
{
    int i = 0;
    int* _Owner _Opt p = &i; //warning 78: cannot assign a non-owner to owner
    free(p);
}
```

### 79 Temporary owner assigned to non-owner
<!-- runnable -->

```c
#pragma ownership enable
void* _Owner _Opt malloc(unsigned long size);

int main(void)
{
    void* _Opt p = malloc(1); //warning 79: cannot assign a temporary owner to non-owner object
    (void)p;
}
```

### 80 Pointer to owner expected
<!-- runnable -->

```c
#pragma ownership enable

void take(int* _Owner _Opt* pp);

int main(void)
{
    int* _Opt p = 0;
    take(&p); //warning 80: pointer to owner expected at argument
}
```

### 81 Owner aliased through a non-owner pointer
<!-- runnable -->

```c
#pragma ownership enable
void free(void* _Owner _Opt p);
void* _Owner _Opt malloc(unsigned long size);

int main(void)
{
    void* _Owner _Opt p = malloc(1);
    void* _Opt* q = &p; //warning 81: owner aliased through a non-owner pointer
    (void)q;
    free(p);
}
```

### 86 Format specifier mismatch
<!-- runnable -->

```c
int printf(const char* fmt, ...);

int main(void)
{
    long n = 1;
    printf("%d", n); //warning 86: format for 'long' is '%ld', not '%d'
}
```

### 87 Implicit conversion to enum
<!-- runnable -->

```c
enum color { RED, GREEN };

int main(void)
{
    enum color c = 1; //warning 87: implicit conversion from 'int' to 'enum color': did you mean 'GREEN'?
    (void)c;
}
```

### 88 Unannotated switch fall-through
<!-- runnable -->

```c
int main(void)
{
    int i = 0;
    switch (i)
    {
    case 0:
        i++; //warning 88: unannotated fall-through between switch labels; add a 'break' or '[[fallthrough]];'
    case 1:
        break;
    }
}
```

### 89 malloc size is not a multiple of the pointee size
<!-- runnable -->

```c
void* _Opt malloc(unsigned long size);

int main(void)
{
    int* _Opt p = malloc(10); //warning 89: argument to 'malloc' is not a multiple of the pointee size (4)
    (void)p;
}
```

### 90–126 Reserved / unused warnings

### 127 Unary minus on unsigned
<!-- runnable -->

```c
int main(void)
{
    unsigned u = 1;
    unsigned v = -u; //warning 127: unary minus operator applied to unsigned type, result still unsigned
    (void)v;
}
```

## Errors 

### 630 Missing terminating '

### 631 Missing terminating "

### 632 Missing end of comment 

### 633 Empty character constant
<!-- runnable -->

```c
int main(void)
{
    char c = ''; //error 633: empty character constant
}
```

### 640 \_View is the default qualifier
<!-- runnable -->

```c
#pragma safety enable

int main() {  
  int * _View i; //error C0640: invalid qualifier for pointer
}
```

### 650 Unexpected compiler error
Some internal error.

### 660 Too many arguments

<!-- runnable -->

```c
void f(void);
int main()
{  
  f(1);
}
```


### 670 Too few arguments
<!-- runnable -->

```c
void f(int i);
int main()
{  
  f();
}
```



### 680 Not found
<!-- runnable -->

```c
int main()
{  
   i = 1;
}
```

### 690 No match for generic selection
<!-- runnable -->

```c
int main(){
    int i;
    _Generic(i, double: 0); // error C0690: no match for generic
}
```

### 700 Subscripted value is neither array nor pointer
<!-- runnable -->

```c
int main(){
    int i;
    i[0]; //error C0700: subscripted value is neither array nor pointer
}
```

### 710 Called object is not function or function pointer
<!-- runnable -->

```c
int main(){
    int i;
    i(); // error C0710: called object is not a function or function pointer
}
```

### 720 Struct member not found
<!-- runnable -->

```c
struct X{int a;};

int main()
{
  struct X x;
  x.b = 1; //error C0720: member 'b' not found in 'struct X'
  return 0;
}
```

### 730 Structure or union required
<!-- runnable -->

```c
int main() {  
  int a;
  a.a = 1; // error C0730: structure or union required
}
```

### 740 Struct is incomplete

<!-- runnable -->

```c
struct X;
int main(){
    struct X x;
}
```

### 750  case label or default not within a switch statement
<!-- runnable -->

```c
int main() {
 case 1:;
}
```

### 760 'break' statement not in loop or switch statement

<!-- runnable -->

```c
int main() {
 break;
}
```

### 770 'continue' statement not in loop statement

<!-- runnable -->

```c
int main() {
 continue;
}
```


### 780 Indirection requires pointer operand
<!-- runnable -->

```c
int main() {
    int i;
    *i = 1; //error C0780: indirection requires pointer operand
}
```

### 790 Invalid token
<!-- runnable -->

```c
#define F(a) ## a //error 790: '##' cannot appear at the beginning of a replacement list
```

### 800 Expected struct type (not emitted)
Reserved (`C_ERROR_EXPECTED_STRUCT_TYPE`). Not emitted by the current compiler.

### 810 Expected type name (not emitted)
Reserved (`C_ERROR_EXPECTED_TYPE_NAME`). Not emitted by the current compiler.

### 820 Left operand is not arithmetic
<!-- runnable -->

```c
struct s { int i; };
int main(void)
{
    struct s a = {0};
    return a * 2; //error 820: left is not an arithmetic type
}
```

### 830 Right operand is not arithmetic
<!-- runnable -->

```c
struct s { int i; };
int main(void)
{
    struct s a = {0};
    return 2 * a; //error 830: right is not an arithmetic type
}
```

### 840 Left operand is not integer
<!-- runnable -->

```c
int main(void)
{
    double d = 1.0;
    return d % 2; //error 840: left is not an integer type
}
```

### 850 Right operand is not integer
<!-- runnable -->

```c
int main(void)
{
    double d = 1.0;
    return 2 % d; //error 850: right is not an integer type
}
```

### 860 Invalid type
<!-- runnable -->

```c
enum E { A = 1.5 }; //error 860: enumerator initializer must be integer
```

### 870 Left operand is not scalar
<!-- runnable -->

```c
struct s { int i; };
int main(void)
{
    struct s a = {0};
    return a && 1; //error 870: left operator is not scalar
}
```

### 880 Right operand is not scalar
<!-- runnable -->

```c
struct s { int i; };
int main(void)
{
    struct s a = {0};
    return 1 && a; //error 880: right operator is not scalar
}
```

### 890 Incompatible pointer types
<!-- runnable -->

```c
int main(void)
{
    int i = 0;
    double d = 0;
    int* p = &i;
    double* q = &d;
    return p == q; //error 890: incompatible pointer types
}
```

### 900 Assignment of function
<!-- runnable -->

```c
int main()
{
    main = 0;    
}
```

### 910 Assignment to array expression

<!-- runnable -->

```c
int main()
{  
    int a[2] = {0};
    a = 1; //error C0910: assignment to expression with array type
}
```

### 920 Assignment of read-only object

Reported for any attempt to modify a const-qualified lvalue: assignment,
compound assignment, and `++`/`--` (C11 6.5.2.4p1 and 6.5.3.1p1 both require a
*modifiable* lvalue). Reaching the object through a const pointer or a const
aggregate counts -- per C11 6.7.3p9 a qualifier on an array type qualifies the
element type, so `p->arr[0]` is const when `p` is.

<!-- runnable -->

```c
int main()
{
    const int i = 1;
    i = 2; 
}

```

<!-- runnable -->

```c
struct X { int arr[4]; };

void f(const struct X* p)
{
    p->arr[0] = 1; //error C0920: assignment of read-only object
}

void g(void)
{
    const int i = 1;
    i++;           //error C0920: increment of read-only object
}
```

### 930 lvalue required as left operand of assignment (not emitted)
Reserved (`C_ERROR_LVALUE_ASSIGNMENT`). Not emitted by the current compiler. Assignment to a non-lvalue is now reported as error 1230.

### 940 Condition must have scalar type


<!-- runnable -->

```c
int main(){
    struct X {int i;} x;
    x ? 1 : 0; //error C0940: condition must have scalar type
}
```

### 950 Incompatible types
<!-- runnable -->

```c
struct a { int i; };
struct b { int i; };
int main(void)
{
    struct a x = {0};
    struct b y = x; //error 950: incompatible types
}
```

### 960 Expected constant expression

<!-- runnable -->

```c
void f(int i)
{
    switch (i)
    {
        case i: //error C0960: expected constant expression
        break;
    }
}
```

### 970 Unexpected token
<!-- runnable -->

```c
int main)
```

### 980 Cannot combine with previous 'long long'
<!-- runnable -->

```c
long long long i; //error 980: cannot combine with previous 'long long' declaration specifier
```

### 990 Expected declaration
<!-- runnable -->

```c
int main(void)
{
    return 0;
}
} //error 990: expected declaration not '}'
```

### 1000 Static/type qualifiers not allowed here
<!-- runnable -->

```c
int main(void)
{
    int a[static 3]; //error 1000: static or type qualifiers are not allowed in non-parameter array declarator
}
```

### 1010 Owner qualifier can only be used with pointers (not emitted)
Reserved (`C_ERROR_OBJ_OWNER_CAN_BE_USED_ONLY_IN_POINTER`). Not emitted by the current compiler.

### 1020 Redeclaration error
<!-- runnable -->

```c

#pragma safety enable
int a;
double a; //error C1020: conflicting types for 'a' (int)
```

### 1030 Tag type mismatch with previous declaration
<!-- runnable -->

```c
struct s { int i; };
union s u; //error 1030: use of 's' with tag type that does not match previous declaration.
```

### 1040 type specifier or qualifier expected
<!-- runnable -->

```c
struct X7
{
    goto  a; //error C1040: type specifier or qualifier expected
};

```

### 1050 Multiple enum definitions (not emitted)
Reserved (`C_ERROR_MULTIPLE_DEFINITION_ENUM`). Not emitted by the current compiler.

### 1060 static_assert failed
<!-- runnable -->

```c
int main()
{  
  static_assert(1 == 2); // error C1060: static_assert failed
}
```

### 1070 override_state error (not emitted)
Reserved (`C_ERROR_STATIC_SET`). Not emitted by the current compiler.

### 1080 Static-state analysis failed (not emitted)
Reserved (`C_FLOW_ANALIZER_ERROR_STATIC_STATE_FAILED`). Not emitted by the current compiler.

### 1090 Unbalanced attribute
<!-- runnable -->

```c
[[deprecated(]] void f(void); //error 1090: expected ']' before ')'
```

### 1100 Unexpected end of file (not emitted)
Reserved (`C_ERROR_UNEXPECTED_END_OF_FILE`). Not emitted by the current compiler. Unexpected end of file is now reported as error 970.

### 1110 throw used outside try block
<!-- runnable -->

```c
int main()
{  
 throw; //error C1110: throw statement not within try block
}
```

### 1120 void function returning a value
<!-- runnable -->

```c
void f(){
    return 1; //error C1120: void function 'f' should not return a value
}
```

### 1121 Non-void function missing return value
<!-- runnable -->

```c
int f(){
  return; //error C1121: non void function 'f' should return a value
}
```

### 1130 Argument size smaller than parameter size
<!-- runnable -->

```c
void f(int a[10]);
int main(void)
{
    int b[5];
    f(b); //error 1130: argument of size [5] is smaller than parameter of size [10]
}
```

### 1140 Token invalid in preprocessor expression
<!-- runnable -->

```c
#if ()
#endif
```

### 1150 File not found (preprocessor)
<!-- runnable -->

```c
#include <file>
```

### 1160 Missing parenthesis
<!-- runnable -->

```c
#define F(a) a
int i = F((1); //error 1160: missing )
```

### 1170 Expression error
<!-- runnable -->

```c
#if 1+
#endif
```


### 1180 Preprocessor error directive
<!-- runnable -->

```c
#error message
```



### 1190 Too few arguments to macro
<!-- runnable -->

```c
#define F(a, b) a
int main()
{
    F(1);  //error C1190: too few arguments provided to function-like macro invocation
}
```

### 1191 Too many arguments to macro
<!-- runnable -->

```c
#define F(a, b) a
int main()
{
    F(1, 2, 3);  //error C1191: too many arguments provided to function-like macro invocation
}
```

### 1200 jump out of defer

<!-- runnable -->

```c

int main()
{    
    _Defer 
    {
        goto target;  // constraint violation
    }

target:
    return 1;
}
```


### 1201 jump over defer
<!-- runnable -->

```c
int main(void)
{
    int i = 0;
    switch (i)
    {
        _Defer { i++; } //error 1201: switch is jumping over or into defer
    case 0:
        break;
    }
}
```

### 1202 jump over VLA

<!-- runnable -->

```c
void f(int n)
{
    goto  target;
    int a[n];    
    target:
}
```


### 1210 Missing macro argument
```c
int main()
{  
//TODO
}
```
### 1220 Address of register variable
```c
int main()
{  
    register int a;
    &a; //error C1220: address of register variable 'x' requested
}
```
### 1230 Operator requires lvalue
<!-- runnable -->

```c
int main(void)
{
    1 = 2; //error 1230: lvalue required as left operand of assignment
}
```

### 1240 Character literal too large
```c
int main()
{  
//TODO
}
```
### 1250 Pragma error (not emitted)
Reserved (`C_ERROR_PRAGMA_ERROR`). Not emitted by the current compiler.

### 1260 Out of memory
Internal compiler error

### 1270 Invalid storage size
<!-- runnable -->

```c
int a[-2]; // error C1270: sizeof 'a' is too large
```

<!-- runnable -->

```c
int main() {
  int a[2147483647]; //error C1270: sizeof 'a' is too large
}
```

### 1280 Returning owner to non-owner (not emitted)
Reserved (`C_ERROR_RETURN_LOCAL_OWNER_TO_NON_OWNER`). Not emitted by the current compiler. This case is now reported as warning 78.

### 1290 auto requires a single declarator
<!-- runnable -->

```c
int main()
{
   auto  * p = 0; //error C1290: 'auto' requires a plain identifier
}
```

### 1300 Multiple incompatible specifiers

<!-- runnable -->

```c
long short s;
```

### 1310 Increment operator cannot be used on owner
<!-- runnable -->

```c
#pragma safety enable
int main(){

    int * _Owner _Opt p = 0;
    p++; //error C1310: operator ++ cannot be used in _Owner pointers
}
```


### 1320 Decrement operator cannot be used on owner
<!-- runnable -->

```c
#pragma safety enable
int main(){

    int * _Owner _Opt p = 0;
    p--; //error C1320: operator -- cannot be used in owner pointers
}
```

### 1330 Preprocessor division by zero
<!-- runnable -->

```c
#if 1/0
#endif
```


### 1340 non-pointer to pointer error
<!-- runnable -->

```c

void f(int * p);

int main() {
    int i;
    f(i); //error C1340: non-pointer to pointer   
}
```

### 1350 Literal overflow
<!-- runnable -->

```c
int main(void)
{
    unsigned long long i = 99999999999999999999; //error 1350: integer literal is too large to be represented in any integer type
    (void)i;
}
```

### 1360 Character not encodable in one code unit
<!-- runnable -->

```c
int main()
{
   unsigned char c = u8'ç';// error C1360: character not encodable in a single code unit.
}

```


### 1370 Multi-character literal error
<!-- runnable -->

```c
int main(void)
{
    int i = u8'ab'; //error 1370: Unicode character literals may not contain multiple characters.
    (void)i;
}
```

### 1380 Invalid token
<!-- runnable -->

```c
int main(void)
{
    int i = 0x; //error 1380: invalid number
    (void)i;
}
```

### 1390 Invalid argument to \_Countof
<!-- runnable -->

```c
int main(){
  int a;
  _Countof(a); //error C1390: argument of _Countof must be an array
}
```


### 1400 return used inside defer
<!-- runnable -->

```c
int main(){  
  defer return 0; //error C1400: return cannot be used inside defer statement
}
```

### 1410 Function returns function type
<!-- runnable -->

```c
int f()(int){} //error C1410: function returning function
```

### 1420 Function returns array type
<!-- runnable -->

```c
int f()[2]{} //error C1420: function returning array
```

### 1430 Label not defined
<!-- runnable -->

```c
int main(){
    goto A; //error C1430: label 'A' used but not defined
}
```


### 1440 Duplicate label
<!-- runnable -->

```c
int main(){
    A:
    A: // error C1440: duplicated label 'A'
}
```

### 1450 Duplicate case label
<!-- runnable -->

```c
void f(int i)
{
    switch(i)
    {
        case 1:
        break;
        case 1: //error C1450: duplicate case '1'
        break;
    }
}
```

### 1560 Array subscript is not an integer
<!-- runnable -->

```c
int main(){
    int a[2];
    a[1.0] = 1; //error C1560: array subscript is not an integer
}
```

### 1570 Duplicate default generic association
```c
int main()
{  
//TODO
}
```
### 1780 Multiple default labels in switch
<!-- runnable -->

```c
void f(int i)
{
    switch(i)
    {
        default:
        break;
        default: //error C1780: multiple default labels in one switch
        break;
    }
}
```


### 1790 Pointer to floating type

<!-- runnable -->

```c
int main()
{
    int *p = 0;
    double d = (double) p; //error C1790: pointer type cannot be converted to any floating type
}
```

### 1800 Floating type converted to pointer

<!-- runnable -->

```c
int main()
{
    int *p = (int*)1.2; //error C1800: A floating type cannot be converted to any pointer type
}
```

### 1810 nullptr cast error

<!-- runnable -->

```c
int main()
{
    int i = (int)nullptr; // error C1810: cannot cast nullptr_t to this type
}
```

### 1820 Macro redefinition (not emitted)
Reserved (`C_ERROR_MACRO_REDEFINITION`). Not emitted by the current compiler. Macro redefinition is now reported as warning 16.

### 1830 Invalid preprocessing directive

<!-- runnable -->

```c
#blablabla //error C1830: invalid preprocessor directive '#blablabla'
```

### 1840 Function cannot be a member

<!-- runnable -->

```c
struct X
{
    void f(); //error C1840: members having a function type are not allowed
};
```

### 1850 Non-integral enum type

<!-- runnable -->

```c
enum E : double {A}; //error C1850: expected an integer type
```

### 1860 Requires compile-time constant

<!-- runnable -->

```c
int a;
int b = a; //error C1860: requires a compile time object
int main()
{    
}
```



### 1870 Outer scope error

<!-- runnable -->

```c

int main()
{
    int i;
	int dup() { return i * 2; } //error C1870: 'i' cannot be evaluated in this scope
    return dup();
}

```

### 1880 Variably modified member
<!-- runnable -->

```c
void f(int n)
{
    struct s { int a[n]; }; //error 1880: Variably modified types cannot be used as members of a structure or union.
}
```

### 1890 Variably modified type with static storage
<!-- runnable -->

```c
void f(int n)
{
    static int a[n]; //error 1890: variably modified type with static storage
}
```

### 1900 typeof on a bit-field
<!-- runnable -->

```c
struct s { int b : 3; };

int main(void)
{
    struct s x = {0};
    typeof(x.b) y; //error 1900: typeof used in bit-field
}
```

### 1910 Initialized variable-sized object
<!-- runnable -->

```c
void f(int n)
{
    int a[n] = {1}; //error 1910: variable-sized object may not be initialized except with an empty initializer
}
```

### 1920 Include path too long

`#include` path exceeds the maximum path length.

### 1930 Write qualifier on a const pointee
<!-- runnable -->

```c
#pragma safety enable
struct s { int i; };
void clear(_Clear const struct s* p) //error 1930: _Clear pointee cannot also be const
{
}
```

### 1940 Write qualifier must qualify the pointee
<!-- runnable -->

```c
#pragma safety enable
struct s { int i; };
void clear(struct s _Clear p); //error 1940: _Clear must be used only at the pointed object
```

### 1950 constexpr value not representable
<!-- runnable -->

```c
int main(void)
{
    constexpr unsigned char c = 300; //error 1950: constant expression is not exactly representable in type
}
```

### 1960 struct/union comparison
<!-- runnable -->

```c
struct s { int i; };
int main(void)
{
    struct s a = {0}, b = {0};
    return a == b; //error 1960: struct/union comparison illegal
}
```

### 1970 Invalid use of void
<!-- runnable -->

```c
int main(void)
{
    void v; //error 1970: this use of 'void' is not valid
}
```

### 1980 extern with initializer at block scope
<!-- runnable -->

```c
int main(void)
{
    extern int i = 1; //error 1980: 'i': cannot initialize extern variables with block scope
}
```

### 1990 void parameter not alone
<!-- runnable -->

```c
void f(void, int i); //error 1990: 'void' must be the first and only parameter if specified
```

### 2000 Operator cannot be applied
<!-- runnable -->

```c
struct s { int i; };
int main(void)
{
    struct s a = {0};
    a = -a; //error 2000: operator cannot be applied to an operand of the given type
}
```

### 2010 typedef used for function definition
<!-- runnable -->

```c
typedef int F(void);
F f { return 0; } //error 2010: typedef cannot be used for function definition
```

### 2020 Duplicate type qualifier
<!-- runnable -->

```c
const const int i = 0; //error 2020: same type qualifier used more than once
```

### 2030 typedef missing tag name
<!-- runnable -->

```c
typedef struct; //error 2030: 'typedef': missing tag name
```

### 2040 Redefinition as typedef
<!-- runnable -->

```c
int x;
typedef int x; //error 2040: 'x': redefinition; symbol cannot be overloaded with a typedef
```
