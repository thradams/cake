## Warnings

Warnings can be enabled with `-w0123` where `123` is the warning number,
or disabled with `-wd0123`.

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
enum E1 {A};
enum E2 {B};
int main()
{    
   enum E1 = B;
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
//-w011
int main() { //  warning C0011: not following correct brace style
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


### 14 String was sliced
<!-- runnable -->

```c
int main()
{  
//TODO
}
```


### 15 Discarded qualifiers

<!-- runnable -->

```c
char* f(){
    static const char s[] = "a";
    return s; //warning C0015:  discarding const
}
```


### 16 (unused)


### 17 Uninitialized variable
<!-- runnable -->

```c
int main()
{  
 int i;
 int j = i;
}
```


### 18 Returning address of local variable (TODO)
<!-- runnable -->

```c
int * f()
{  
    int i;
    return &i;
}
```


### 19 Missing address-of operator
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 20 Array indirection issue
<!-- runnable -->

```c
void f(int a[])
{
    *a =1; // warning C0020: array indirection
}
```


### 21 (unused)

### 22 Using object without being owner
<!-- runnable -->

```c
int main()
{  
//TODO
}
```


### 23 Using temporary owner incorrectly
<!-- runnable -->

```c
int main()
{  
//TODO
}
```


### 24 Move-assignment to non-owner
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 25 Assigning non-owner to owner
<!-- runnable -->

```c
#pragma safety enable

void  f(int * _Owner);
int main()
{
  int * _Opt p = 0;
  f(p); // warning C0025: cannot assign a non-owner to owner
}
```


### 26 Discarding an owner
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

### 28 Non-null flow violation

<!-- runnable -->

```c
#pragma nullable enable

void f(int *p)
{
  if (p) //warning C0028: pointer is always not-null
  {   
  }
}
int main() {}
```

<!-- runnable -->

```c
#pragma nullable enable

int main() {
  int * _Opt p = 0;
  if (p) //warning C0028: pointer is always null
  {   
  }
}
```
See [object lifetime](ownership.md)

### 29 pointed object was not released (flow analysis)
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

### 30 Uninitialized value (flow analysis)

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


### 31 Lifetime has ended (flow analysis)
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
    p->i = 0; //warning C0031: object lifetime ended
}

```


See [object lifetime](ownership.md)


### 32 Object already moved (flow analysis)
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


### 33 Null dereference (flow analysis)

This warning indicates that your code dereferences a potentially null pointer. 

<!-- runnable -->

```c
#pragma safety enable
int main()
{
  int * _Opt p = 0;
  *p =1; //warning C0033: dereference a NULL object
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


### 36 Division by zero (flow analysis)
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
int main()
{  
  int i = 'abc';
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

### 60 Null pointer constant to non-nullable pointer

<!-- runnable -->

```c
#pragma safety enable

int main() {  
  int * p2 = nullptr; //warning C0060: cannot convert a null pointer constant to non-nullable pointer
}
```

### 61 Cast to same type (inactive)
<!-- runnable -->

```c
int main()
{  
  int i = (int) 0;
}
```


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

### 63 Signed to unsigned
<!-- runnable -->

```c
void f(unsigned i) { (void)i; }

int main(void) {
    int x = -5;
    f(-1);
    f(x);
    unsigned u = -2;
    return 0;
}
```


### 64–127 Reserved / unused warnings

## Errors 

### 630 Missing terminating '

### 631 Missing terminating "

### 632 Missing end of comment 

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
int main()
{  
//TODO
}
```

### 800 Expected struct type
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 810 Expected type name
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 820 Left operand is not arithmetic
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 830 Right operand is not arithmetic
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 840 Left operand is not integer
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 850 Right operand is not integer
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 860 Invalid type
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 870 Left operand is not scalar
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 880 Right operand is not scalar
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 890 Incompatible pointer types
<!-- runnable -->

```c
int main()
{  
//TODO
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
<!-- runnable -->

```c
int main()
{
    const int i = 1;
    i = 2; 
}

```

### 930 lvalue required as left operand of assignment
<!-- runnable -->

```c
int main()
{  
  1 = 2; //error C1230: lvalue required as left operand of assignment
}
```

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
int main()
{  
//TODO
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
int main()
{  
//TODO
}
```

### 990 Expected declaration
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 1000 Static/type qualifiers not allowed here
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 1010 Owner qualifier can only be used with pointers
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

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
int main()
{  
//TODO
}
```

### 1040 type specifier or qualifier expected
<!-- runnable -->

```c
struct X7
{
    goto  a; //error C1040: type specifier or qualifier expected
};

```

### 1050 Multiple enum definitions
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 1060 static_assert failed
<!-- runnable -->

```c
int main()
{  
  static_assert(1 == 2); // error C1060: static_assert failed
}
```

### 1070 override_state error
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 1080 Static-state analysis failed

<!-- runnable -->

```c
#pragma safety enable

int f();

int main() {   
    int i = f();
    assert_state(i, "not-zero"); //error C1080: assert_state failed
} 
```


### 1090 Unbalanced attribute
<!-- runnable -->

```c
int main()
{  
//TODO
}
```

### 1100 Unexpected end of file

<!-- runnable -->

```c
int main()
{  //error C0970: unexpected end of file

```

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
int main()
{  
//TODO
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
int main()
{  
//TODO
}
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
void f(int n)
{
    goto  target;
    _Defer {};    
    target:
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
int main()
{  
 1 == 2;
}
```

### 1240 Character literal too large
```c
int main()
{  
//TODO
}
```
### 1250 Pragma error
<!-- runnable -->

```c
int main()
{  
//TODO
}
```


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

### 1280 Returning owner to non-owner
<!-- runnable -->

```c
#pragma safety enable

int * g();

int * _Owner f(){
  return g(); //warning C0025: cannot assign a non-owner to owner
}
```

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
int main()
{  
//TODO
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
int main()
{  
//TODO
}
```


### 1380 Invalid token
<!-- runnable -->

```c
int main()
{  
//TODO
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

### 1820 Macro redefinition

<!-- runnable -->

```c
#define A 1
#define A 2 //error C1820: macro redefinition
```

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

