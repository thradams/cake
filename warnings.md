## Warnings

Warnings can be enabled with `-w0123` where `123` is the warning number,
or disabled with `-wd0123`.

All warning are configurable. Cake has errors, warnings and notes.
We can make a warning to be a note or error using pragma.

```c
#pragma CAKE diagnostic push
#pragma CAKE diagnostic error "C0004"
    

    /*
      c04 is not reported as error
      code..
    
    */

#pragma CAKE diagnostic pop

```


### 1 \#warning directive (preprocessor)
```c
#warning message
```
<button onclick="Try(this)">try</button>

### 2 Unused variable

```c
// -w02
void f()
{
    int i;    
}
```
<button onclick="Try(this)">try</button>

### 3 Deprecated

```c
[[deprecated]] void f2() {}

int main(void)
{
    f2();
}
```

<button onclick="Try(this)">try</button>

### 4 Enum conversion issue

```c
enum E1 {A};
enum E2 {B};
int main()
{    
   enum E1 = B;
}
```

<button onclick="Try(this)">try</button>

### 5 Suspicious address usage

```c
int main()
{    
}

```

<button onclick="Try(this)">try</button>

### 6 Unused function parameter (disabled by default)

```c
// -w06
void f(int i) {}
```
<button onclick="Try(this)">try</button>
### 7 Declarator hides previous declaration

```c
void f() {
    int i;
    {
      int i;
    }
}
```
<button onclick="Try(this)">try</button>
### 8 typeof used on array parameter

```c
void f(int a[])
{
    typeof(a) p;
}
```
<button onclick="Try(this)">try</button>
### 9 ignoring the result of _Owner type 

```c
#pragma safety enable
int *_Owner f();
int main()
{
  f();  //warning C0009: ignoring the result of _Owner type 
}
```
<button onclick="Try(this)">try</button>

### 10 Expression result not used

```c
int main()
{  
  int a = 0;
  a++;
  
  a; //warning C0010: expression result not used
  1; //warning C0010: expression result not used
 
}
```

<button onclick="Try(this)">try</button>

### 11 Style issue (disabled by default)

```c
//-w011
int main() { //  warning C0011: not following correct brace style
}

```

<button onclick="Try(this)">try</button>

### 12  multi-line comment

```c
int main()
{  
  // \
  a++; 
}
```

<button onclick="Try(this)">try</button>

### 13 Line slicing occurred
```c
int a \
   = 1; // warning C0013: unnecessary line-slicing
}
```

<button onclick="Try(this)">try</button>

### 14 String was sliced
```c
int main()
{  
//TODO
}
```

<button onclick="Try(this)">try</button>

### 15 Discarded qualifiers

```c
char* f(){
    static const char s[] = "a";
    return s; //warning C0015:  discarding const
}
```

<button onclick="Try(this)">try</button>

### 16 (currently not used)


### 17 Uninitialized variable
```c
int main()
{  
 int i;
 int j = i;
}
```

<button onclick="Try(this)">try</button>

### 18 Returning address of local variable (TODO)
```c
int * f()
{  
    int i;
    return &i;
}
```

<button onclick="Try(this)">try</button>

### 19 Missing address-of operator
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 20 Array indirection issue
```c
int main()
{  
//TODO
}
```

<button onclick="Try(this)">try</button>

### 21 (currently not used)

### 22 Using object without being owner
```c
int main()
{  
//TODO
}
```

<button onclick="Try(this)">try</button>

### 23 Using temporary owner incorrectly
```c
int main()
{  
//TODO
}
```

<button onclick="Try(this)">try</button>

### 24 Move-assignment to non-owner
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 25 Assigning non-owner to owner
```c
#pragma safety enable

void  f(int * _Owner);
int main()
{
  int * _Opt p = 0;
  f(p); // warning C0025: cannot assign a non-owner to owner
}
```

<button onclick="Try(this)">try</button>

### 26 Discarding an owner
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 27 Moving a non-owner object
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 28 Non-null flow violation
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 29 Missing destructor (flow analysis)
```c
int main()
{  
//TODO
}
```

<button onclick="Try(this)">try</button>

### 30 Uninitialized value (flow analysis)
```c
int main()
{  
//TODO
}
```

<button onclick="Try(this)">try</button>

### 31 Lifetime has ended (flow analysis)
```c
int main()
{  
//TODO
}
```

<button onclick="Try(this)">try</button>


### 32 Object already moved (flow analysis)
```c
int main()
{  
//TODO
}
```

<button onclick="Try(this)">try</button>

### 33 Null dereference (flow analysis)

```c
#pragma safety enable
int main()
{
  int * _Opt p = 0;
  *p =1; //warning C0033: dereference a NULL object
}
```

<button onclick="Try(this)">try</button>


### 34 (not used flow)

### 35 Nullable converted to non-nullable (flow)
```c
#pragma safety enable

void f(int *p);
int main()
{
  int * _Opt p = 0;
  f(p); // warning C0035: passing a possible null pointer 'p' to non-nullable pointer parameter
}
```

<button onclick="Try(this)">try</button>

### 36 Division by zero (flow analysis)
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
<button onclick="Try(this)">try</button>

### 37 Division by zero (without flow analysis)

```c
int main()
{
    const int i = 0;
    int j = 1/i; //warning C0037: division by zero
}
```

<button onclick="Try(this)">try</button>


### 38 (not used)

### 39 Passing null as array
```c
void f(int a[]){}
int main(){
    f(0); //warning C0039:  passing null as array
}

```

<button onclick="Try(this)">try</button>


### 40 Incompatible enum types
```c
enum E1 {A};
enum E2 {B};
int main()
{    
   enum E1 e = B; //warning C0040:  incompatible types
}
```
<button onclick="Try(this)">try</button>

### 41 Multi-character literal
```c
int main()
{  
  int i = 'abc';
}
```
<button onclick="Try(this)">try</button>

### 42 Out-of-bounds access
```c
int main()
{  
  int a[2]= {0};
  a[2] = 1; //warning C0042: index 2 is past the end of the array
}
```
<button onclick="Try(this)">try</button>

### 43 Assignment to array parameter
```c
void f(int a[]){
    a = 1; // warning C0043: assignment to array parameter
}
```
<button onclick="Try(this)">try</button>

### 44 Conditional is constant (currently disabled)

```c
int main()
{  
  if (1) {
  }
}
```
<button onclick="Try(this)">try</button>

### 45 Suspicious switch statement

```c
int main()
{
    bool b =1;
    switch(b){ //warning C0045: switch condition has boolean value
        case 1:break;
    }
}

```

<button onclick="Try(this)">try</button>

### 46 Unusual null pointer constant
```c
int main()
{
    int * p = '\0'; //warning C0046: unusual expression/type used as null pointer constant
}
```
<button onclick="Try(this)">try</button>

### 47 sizeof applied to array argument
```c
void f(int a[])
{
    sizeof(a); //warning C0047: sizeof applied to array function parameter
}
```
<button onclick="Try(this)">try</button>

### 48 const object not initialized
```c
int main()
{  
  const int i;
}
```
<button onclick="Try(this)">try</button>

### 49 implicit conversion of nullptr constant to 'bool'
```c
void f(bool b){}
int main(){
    f(nullptr); //warning C0049: implicit conversion of nullptr constant to 'bool'
}
```

<button onclick="Try(this)">try</button>

### 50 Implicitly unsigned literal
```c

int main(){
  long long a = 9223372036854775807;
  unsigned long long b = 9223372036854775808; // warning C0050: integer literal is too large to be represented in a signed integer type, interpreting as unsigned
  unsigned long long c = 9223372036854775808ULL;
}

```
<button onclick="Try(this)">try</button>

### 51 Integer overflow
```c

int main(){
  long long a = 9223372036854775807 + 2; //warning C0051: integer overflow
}

```
<button onclick="Try(this)">try</button>

### 52 Invalid array size
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 53 Empty statement
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 54 Incompatible types
```c
void f(double * p);
int main()
{  
    int a;
    f(&a); // warning C0054:  incompatible types
}
```

<button onclick="Try(this)">try</button>

### 55 Unused label
```c
int main()
{  
  A: //warning C0055: label 'A' defined but not used
}
```
<button onclick="Try(this)">try</button>
### 56 Redefining builtin macro

```c
#define __FILE__ 1 //warning C0056: redefining built-in macro
```

<button onclick="Try(this)">try</button>

### 57 Unused function
```c
static void f(){};
int main()
{  
}
```

<button onclick="Try(this)">try</button>

### 58 Boolean comparison issue
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
<button onclick="Try(this)">try</button>

### 59 Expected warning did not occur
```c
int main()
{
    bool b = false;
    bool i = false;
    if ([[cake::w58]] b == i) //warning C0059: warning 'C0058' was not recognized
    {
    }
}
```
<button onclick="Try(this)">try</button>

### 60 Nullable to non-nullable
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 61 Cast to same type
```c
int main()
{  
//TODO
}
```

<button onclick="Try(this)">try</button>

### 62 Too many initializers
```c
int main()
{  
  int a[2] = {1, 2, 3};
}
```
<button onclick="Try(this)">try</button>
### 63 Float out of range
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 64–127 Reserved / unused warnings

---

### 640 Invalid qualifier for pointer
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 650 Unexpected compiler error
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 660 Too many arguments

```c
void f(void);
int main()
{  
  f(1);
}
```

<button onclick="Try(this)">try</button>

### 670 Too few arguments
```c
void f(int i);
int main()
{  
  f();
}
```

<button onclick="Try(this)">try</button>


### 680 Not found
```c
int main()
{  
   i = 1;
}
```
<button onclick="Try(this)">try</button>

### 690 No match for generic selection
```c
int main(){
    int i;
    _Generic(i, double: 0); // error C0690: no match for generic
}
```
<button onclick="Try(this)">try</button>

### 700 Subscripted value is neither array nor pointer
```c
int main(){
    int i;
    i[0]; //error C0700: subscripted value is neither array nor pointer
}
```
<button onclick="Try(this)">try</button>

### 710 Called object is not function or function pointer
```c
int main(){
    int i;
    i(); // error C0710: called object is not a function or function pointer
}
```
<button onclick="Try(this)">try</button>

### 720 Struct member not found
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 730 Structure or union required
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 740 Struct is incomplete
```c
struct X;
int main(){
    struct X x;
}
```
<button onclick="Try(this)">try</button>

### 750 Declarator not found
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 760 Expected declarator name
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 770 Unknown attribute name
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 780 Indirection requires pointer operand
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 790 Invalid token
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 800 Expected struct type
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 810 Expected type name
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 820 Left operand is not arithmetic
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 830 Right operand is not arithmetic
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 840 Left operand is not integer
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 850 Right operand is not integer
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 860 Invalid type
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 870 Left operand is not scalar
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 880 Right operand is not scalar
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 890 Incompatible pointer types
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 900 Assignment of function
```c
int main()
{
    main = 0;    
}
```
<button onclick="Try(this)">try</button>

### 910 Assignment to array expression

```c
int main()
{  
    int a[2] = {0};
    a = 1; //error C0910: assignment to expression with array type
}
```
<button onclick="Try(this)">try</button>

### 920 Assignment of read-only object
```c
int main()
{
    const int i = 1;
    i = 2; 
}

```
<button onclick="Try(this)">try</button>

### 930 lvalue required as left operand of assignment
```c
int main()
{  
  1 = 2; //error C1230: lvalue required as left operand of assignment
}
```
<button onclick="Try(this)">try</button>

### 940 Condition must have scalar type


```c
int main(){
    struct X {int i;} x;
    x ? 1 : 0; //error C0940: condition must have scalar type
}
```
<button onclick="Try(this)">try</button>

### 950 Incompatible types
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 960 Expected constant expression
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 970 Unexpected token
```c
int main)
```
<button onclick="Try(this)">try</button>

### 980 Cannot combine with previous 'long long'
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 990 Expected declaration
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1000 Static/type qualifiers not allowed here
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1010 Owner qualifier can only be used with pointers
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1020 Redeclaration error
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1030 Tag type mismatch with previous declaration
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1040 Missing enum tag name
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1050 Multiple enum definitions
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1060 static_assert failed
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1070 static_set error
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1080 Static-state analysis failed
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1090 Unbalanced attribute
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1100 Unexpected end of file
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1110 throw used outside try block
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1120 void function returning a value
```c
void f(){
    return 1; //error C1120: void function 'f' should not return a value
}
```
<button onclick="Try(this)">try</button>

### 1121 Non-void function missing return value
```c
int f(){
  return; //error C1121: non void function 'f' should return a value
}
```
<button onclick="Try(this)">try</button>

### 1130 Argument size smaller than parameter size
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1140 Token invalid in preprocessor expression
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1150 File not found (preprocessor)
```c
#include <file>
```
<button onclick="Try(this)">try</button>

### 1160 Missing parenthesis
```c
int main()
{  
//TODO
}
```
<button onclick="Try(this)">try</button>

### 1170 Expression error
```c
int main()
{  
//TODO
}
```
### 1180 Preprocessor error directive
```c
#error message
```
### 1190 Too few arguments to macro
```c
int main()
{  
//TODO
}
```
### 1191 Too many arguments to macro
```c
int main()
{  
//TODO
}
```
### 1200 Invalid macro argument
```c
int main()
{  
//TODO
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
```c
int main()
{  
//TODO
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
```c
int main()
{  
//TODO
}
```
### 1260 Out of memory
```c
int main()
{  
//TODO
}
```
### 1270 Invalid storage size
```c
int a[-2]; // error C1270: sizeof 'a' is too large
```

### 1280 Returning owner to non-owner
```c
#pragma safety enable

int * g();

int * _Owner f(){
  return g(); //warning C0025: cannot assign a non-owner to owner
}
```

### 1290 auto requires a single declarator
```c
int main()
{
   auto  * p = 0; //error C1290: 'auto' requires a plain identifier
}
```
### 1300 Multiple incompatible specifiers

```c
long short s;
```

### 1310 Increment operator cannot be used on owner
```c
#pragma safety enable
int main(){

    int * _Owner _Opt p = 0;
    p++; //error C1310: operator ++ cannot be used in _Owner pointers
}
```
### 1320 Decrement operator cannot be used on owner
```c
#pragma safety enable
int main(){

    int * _Owner _Opt p = 0;
    p--; //error C1320: operator -- cannot be used in owner pointers
}
```

### 1330 Preprocessor division by zero
```c
#if 1/0
#endif
```
### 1340 Integer-to-pointer conversion error
```c
int main()
{  
//TODO
}
```
### 1350 Literal overflow
```c
int main()
{  
//TODO
}
```
### 1360 Character not encodable in one code unit
```c
int main()
{
   unsigned char c = u8'ç';// error C1360: character not encodable in a single code unit.
}

```
### 1370 Multi-character literal error
```c
int main()
{  
//TODO
}
```
### 1380 Invalid token
```c
int main()
{  
//TODO
}
```
### 1390 Invalid argument to \_Countof
```c
int main(){
  int a;
  _Countof(a); //error C1390: argument of _Countof must be an array
}
```
### 1400 return used inside defer
```c
int main(){  
  defer return 0; //error C1400: return cannot be used inside defer statement
}
```

### 1410 Function returns function type
```c
int f()(int){} //error C1410: function returning function
```

### 1420 Function returns array type
```c
int f()[2]{} //error C1420: function returning array
```

### 1430 Label not defined
```c
int main(){
    goto A; //error C1430: label 'A' used but not defined
}
```
### 1440 Duplicate label
```c
int main(){
    A:
    A: // error C1440: duplicated label 'A'
}
```
### 1450 Duplicate case label
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
```c
int main()
{  
//TODO
}
```
### 1790 Pointer to floating type

```c
int main()
{    
}
```

### 1800 Floating type converted to pointer

```c
int main()
{    
}
```

### 1810 nullptr cast error

```c
int main()
{    
}
```

### 1820 Macro redefinition

```c
#define A 1
#define A 2 //error C1820: macro redefinition
```

### 1830 Invalid preprocessing directive

```c
#blablabla //error C1830: invalid preprocessor directive '#blablabla'
```

### 1840 Function cannot be a member

```c
struct X
{
    void f(); //error C1840: members having a function type are not allowed
};
```

### 1850 Non-integral enum type

```c
enum E : double {A}; //error C1850: expected an integer type
```

### 1860 Requires compile-time constant

```c
int main()
{    
}
```

### 1870 Outer scope error

```c
int main()
{    
}
```
