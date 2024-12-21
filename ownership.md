  
Last Updated 21 Dez 2024
  
This is a work in progress. Cake source is currently being used to validate the concepts. It's in the process of transitioning to include annotated nullable checks, which was the last feature added.  


## Abstract
  
The objective is to statically check code and prevent bugs, including memory bugs like double free, 
null dereference and memory leaks. New qualifiers have been created to extend the type system and 
insert information at function declarations.

Ultimately, we still have the same language, but with an improved type system that checks new contracts.
These new contracts can be ignored, the language **and existing code patterns** remains unmodified. 


## Concepts

### Nullable Pointers

A nullable pointer is a pointer that be set to a null value, indicating that it doesn't currently reference any object.  

The qualifier `_Opt` explicitly indicates when a pointer is nullable, while the absence of the qualifier implies that a pointer is non-nullable. This qualifier is placed after  `*` in the same way as `const`.

The declaration

```c
char * _Opt strdup(const char * src);
```

says that `strdup` is a function that expects a non nullable pointer as argument and returns a nullable pointer.

Since `_Opt` is new, and the absence of the `_Opt` qualifier indicates that the pointer is non-null, we need to specify where in the code these rules apply. This is accomplished with the `#pragma nullable enable` directive.
After `#pragma nullable enable`, the compiler will assume that the code has been reviewed and that the `_Opt` qualifier has been added where necessary to indicate that the pointer may be null, while it is omitted when the pointer cannot be null

#### Example 1: Warning for Non-Nullable Pointers

```c
#pragma nullable enable  
int main(){
  int * p = nullptr; // warning
}
```

<button onclick="Try(this)">try</button>

In this example, `p` is a non-nullable pointer, since the rules are in effect after `#pragma nullable enable` and the pointer is not qualified with `_Opt`.  
Assign `p` to `nullptr` will generate a warning. 

The `#pragma nullable disable` directive can be used to say "the rules for nullable pointers are NOT enabled in this source" facilitating the transition.

This approach has been used in C#.[1]


#### Example 2: Converting Non-Nullable to Nullable

The conversion from a non-nullable pointer to a nullable one is allowed, as shown below:

```c
#pragma nullable enable  
char * get_name();  
int main(){
  char * _Opt s = get_name(); 
}
```

<button onclick="Try(this)">try</button>

#### Example 3: Diagnostic for Nullable to Non-Nullable Conversion

Consider the following case:

```c
#pragma nullable enable  
  
char * _Opt strdup(const char * src);  
 
void f(char *s);  

int main()
{  
   char * _Opt s1 = strdup("a");
   f(s1); // warning
} 
```

<button onclick="Try(this)">try</button>

In this scenario, `s1` is declared as nullable, but `f` expects a non-nullable argument. This triggers a warning, as the nullable pointer `s1` could potentially be null when passed to `f`. To remove this warning, a null check is required:

```c
  if (s1)
    f(s1); // ok
```

This warning relies on flow analysis, which ensures that the potential nullability of pointers is checked before being passed to functions or assigned to non-nullable variables.

In some cases, the compiler may need a help. Consider this sample.


```c
#pragma safety enable

struct X {
    int * _Opt data;
};

bool is_empty(struct X * p)
{
    return p->data == nullptr;
}

void f(struct X * p) 
{
   if (!is_empty(p)) {
      assert(p->data != nullptr);
      *p->data = 1;
   }
}
```  

When is_empty(p) is true `p->data` is null; otherwise, `p->data` not null. Since the analysis is not inter-procedural, the compiler does not have this information. Adding an assertion will lead the flow analysis to assume that `p->data` is not null and removes the warning.

The problem with this approach is the distance between the place that imposes the post condition and assert. If `is_empty` changes it could potentially invalidate the assert at caller side.

The C++ 26 proposal (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2900r5.pdf) for contracts is being considered to solve this problem. The advantage is that the postconditions are defined in one place. This means that changing the implementation of `is_empty` we update its postconditions in a single location.
  
Using C++ 26 syntax for contracts we have (This may not be valid in C++ 26 but here is how it could be used here)

```c
bool is_empty(struct X * p) 
  pos(r: r && p->data == nullptr)
  pos(r: !r && p->data != nullptr);
```
  
`pos` indicates post condition. `r:` indicates the result of `is_empty´.   

- if the result is true then p->data is null.
- if the result is false then p->data is not null.
  
The expectation is now that the flow analysis undestand `p->data` is not null inside the if.  If the contract is changed, then the warning may be back and this is exactly what we want.

```c
void f(struct X * p)
{
   if (!is_empty(p)) {
      *p->data = 1;
   }
}
```  

#### Non nullable members initialization
  
Non-nullable member initialization has similarities to const member initialization. One difference is that const members cannot be changed after declaration even if the declaration does not initialize it.
  
For instance:

```c  
struct X { const int i; };
int main(){
    struct X x;
    x.i = 1; //error
}
```

<button onclick="Try(this)">try</button>

The non-nullable on the other hand can.

```c
#pragma nullable enable  
char * _Opt strdup(const char * src);  

struct X {  char * text; };  

void f() {  
   char * _Opt s = strdup("a");
   if (s == nullptr)
     return;
   struct X x;
   x.text = s; //ok
}
```

<button onclick="Try(this)">try</button>

How do we know when the object is fully constructed?   
We don't need to.  Attempting to read an uninitialized or partially initialized object will result in a warning.

For instance.

```c
#pragma nullable enable  
struct X {  char * text; };  

struct X f() {  
   struct X x;
   return x;
}
```

<button onclick="Try(this)">try</button>
  
More initialization patterns

```c
#pragma nullable enable  
char * _Opt strdup(const char * src);  
  
struct X {  char * text; };  

void f() {  
  struct X x = {}; //warning  
}  
void g() {  
   struct X x = {.text = strdup("a") }; //warning
}
void h() {  
   char * _Opt s = strdup("a");
   if (s == nullptr)
     return;
   struct X x = {.text = s }; //ok
}
```

`malloc` has a built-in semantics indicating the object is uninitialized. 

```c
#pragma nullable enable  
char * _Opt strdup(const char * src);  
void * _Opt malloc(unsigned int sz);

struct X {  char * text; };  

void f() {     
   struct X * _Opt pX = malloc(sizeof *pX);
   if (pX)
   {
      char * _Opt s = strdup("a");
      if (s != nullptr){
        pX->text = s; //ok
      }     
   }
}
```

<button onclick="Try(this)">try</button>
  
 `calloc` has a built in semantics indicating the object is zero-initialized. 

```c
#pragma nullable enable  
char * _Opt strdup(const char * src);  
void * _Opt calloc(unsigned int n, unsigned int sz);

struct X {  char * text; };  

void f() {     
   struct X * _Opt pX = calloc(1, sizeof *pX); //warning
   if (pX)
   {          
   }
}
```
<button onclick="Try(this)">try</button>


In some cases it may be useful to make the non-nullable members as nullable. Applying `_Opt` qualifier to structs makes all members nullable.

For instance, the previous calloc sample can be written as.

```c  
#pragma nullable enable  
char * _Opt strdup(const char * src);  
void * _Opt calloc(unsigned int n, unsigned int sz);

struct X {  char * text; };  

void f() {     
   _Opt struct X * _Opt pX = calloc(1, sizeof *pX); //ok
   if (pX)
   {
      char * _Opt s = strdup("a");
      if (s != nullptr){
        pX->text = s; //ok
      }     
   }
}

```    
  
<button onclick="Try(this)">try</button>

#### mutable

In the same way we removed the implicit non-nullable  qualifier, we also could remove the const qualifier. The anti const qualifier could be named as `mutable`.

But instead of having anti-const and anti-non-nullable qualifier the mutable has been considered to remove both.
  
Consider the following code example:

```c  
struct X {
  const char * const name; // const and non-nullable
};  

struct X * _Opt makeX(const char* name)
{
  mutable struct X * p = calloc(1, sizeof *p);  
  if (p == NULL) 
    return NULL;
  
  char * _Opt temp = strdup(name);
  if (temp == NULL)
    return NULL;  

  p->name = temp;  // OK!!
  return p;
}
```

>OBS: mutable qualifier is not yet implemented. 

Note: The syntax used in the C++ 26 proposal for contracts may be utilized in the future to eliminate the need for built-in semantics for `malloc` and `calloc`.

### Object lifetime checks  
From the C23 standard:

"The *lifetime* of an object is the portion of program execution during which storage is guaranteed
to be reserved for it. An object exists, has a constant address, and retains its last-stored value
throughout its lifetime) If an object is referred to outside of its lifetime, the **behavior is undefined**"

To conceptually manage the storage and object lifetime we will use the concept of ownership.

#### Owner references
An **owner reference** is an object referencing another object and managing its lifetime. 

The most common type of owner reference are pointers to *allocated* objects.  We can call then just **owner pointers**.  
 
**Sample - Owner Pointer to FILE**

```c
#pragma safety enable

#include <stdio.h>

int main()
{
    FILE *_Owner _Opt f = fopen("file.txt", "r");
    if (f)
    {
       fclose(f);
    }
}
```

<button onclick="Try(this)">try</button>

##### \#pragma ownership enabled

Object lifetime checks are enabled with the `#pragma ownership enable`.      
Qualifiers like `_Owner` can be used when ownership is disabled but they are ignored.

##### \#pragma safety enabled  

`#pragma safety enable` is equivalent of :

```c
#pragma nullable enable
#pragma ownership enable
```

Sample:  

If the programmer incorrectly assumes that `fclose` accepts NULL.

```c
#pragma safety enable

#include <stdio.h>

int main()
{
    FILE * _Owner _Opt f = fopen("file.txt", "r");
    if (f)
    {
    }
    //warning: assignment of possible null object 'p' to non-opt pointer [-Wnullable-to-non-nullable]
    fclose(f);
}    
```

<button onclick="Try(this)">try</button>


**Rule:** An **owner reference** is always the unique owner of the referenced object. As a consequence when owner references are copied the ownership is transferred.

**Rule:** Before the end of its lifetime, owner references must move the ownership of the objects they own.

The cake ownership model does not have the concept of a destroyed object. Instead, everything is viewed as a transformation, where the object is broken into 
smaller parts and those parts are moved.

Sample

```c  
#pragma safety enable

#include <stdio.h>

int main()
{
	FILE * _Owner _Opt f = fopen("file.txt", "r");
	FILE * _Owner _Opt f2 = f; /*MOVED*/
	if (f2)
       fclose(f2); /*MOVED*/
}
```
<button onclick="Try(this)">try</button>

Invoking a function `fclose` is analogous to assignment of the argument `f2`, resulting in the transfer of ownership of `f2` to the function parameter.  

Sample - Declaration of fopen and fclose

```c
FILE * _Owner _Opt fopen( const char *filename, const char *mode );
void fclose(FILE * _Owner p); /*p is not nullable*/
```

#### Non-pointer owner references

We can have other types of **owner references**.   

For instance, Berkeley sockets use an integer to identify the socket.

Sample

```c
 _Owner int server_socket = socket(AF_INET, SOCK_STREAM, 0);
 /*...*/
 close(server_socket);
```

> Note: The location and usage of the qualifier \_Owner is similar to the const qualifier. For pointers, it goes after *, and for this socket sample, it can be before int. The \_Owner qualifier belongs to the object (memory)that holds the reference.

When a struct or union have at least one owner object we can say the struct is a owner object too.

**Rule:** Owner objects cannot be discarded.

```c  
#pragma safety enable

#include <stdio.h>

int main() {  
  fopen("file.txt", "r"); //warning   
}
```
<button onclick="Try(this)">try</button>

**Rule:** A non-owner object cannot be copied to a owner object.

**Rule:** The null pointer constant can be used to initialize owner objects.

**Sample**

```c
#pragma safety enable

typedef int T;
  
T * f(); /*returning non owner*/  

int main() {  
   T * _Owner p = f();      //ERROR   
   T * _Owner _Opt _p2 = 0; //OK
}
```
<button onclick="Try(this)">try</button>

#### View references

A **view reference** is an object referencing another object without managing its lifetime. 

**Rule:** The lifetime of the referenced object must be longer than the lifetime of the view reference.

Sample:

```c
#pragma safety enable

struct X
{
    int i;
}
int main(){

  struct X * _Opt p = nullptr;
  {
    struct X x = {};
    p = &x;
  }
  p->i = 1; //warning
}
```
<button onclick="Try(this)">try</button>

The most common view references are pointers called **view pointers**. 

The view qualifier is not necessary for pointers, since it's the default behavior. The usage of _View in pointers  are forbidden to avoid the propagation of more than one style.  

When an owner object is copied to a view object, the ownership is not transferred.
  
**Sample**

```c
#pragma safety enable

#include <stdio.h>

void use_file(FILE * f) {}

int main() {
    FILE * _Owner _Opt f = fopen("file.txt", "r");
    if (f) {
        use_file(f); //not moved
        fclose(f);
    }
}
```
<button onclick="Try(this)">try</button>


When a **\_View** qualifier is used in structs, it makes all members as view objects. 
 
**Sample - A view parameter**

```c
#pragma safety enable

#include <stdlib.h>

struct X {   
  char * _Owner _Opt text;   
};  

void f(_View struct X x) { /*...*/ }  

int main() {
    struct X x = {};
    f(x); /*NOT MOVED*/    
    free(x.text);
}
```

<button onclick="Try(this)">try</button>

> Note: It is interesting to compare against const qualifier. While const adds a qualifier "const" "\_View" removes the qualifier "\_Owner".

#### Returning a pointer to a view object
  
**Rule:** We cannot return a view pointer to objects with automatic storage duration from the function scope.

```c
#pragma safety enable

int * f()
{
   int a = 1;
   return &a; //ERROR
}
```  
  <button onclick="Try(this)">try</button>

But we can return a view pointer to objects with static, thread and allocated duration.
  
Sample:
```c
static int a = 1;
int * f()
{   
   return &a; // OK
}  
```  
  
  <button onclick="Try(this)">try</button>

We can return view pointers to objects pointed by parameters because they are from one scope above.  We cannot return the address of the parameters.

```c
int * f2(int *p) {
   return p; //OK
}
```

<button onclick="Try(this)">try</button>

Now consider:

```c   
#include <stdio.h>

int * max(int * p1, int * p2) {  
 return *p1 > *p2 ? p1 : p2;
}

int main(){  
   int * p = NULL;
   int a  = 1;
   {
      int b = 2;
      p = max(&a,  &b);
   }
   printf("%d", *p);
}
```
<button onclick="Try(this)">try</button>

Examining the implementation reveals that the returned view pointer's lifetime can be that of either 'a' or 'b'.

This check is missing at cake but the idea is to make `a` and `b` suspects. Considering the lifetime of `b` smaller than `p` then the programmer needs to tell the compiler that `p` cannot be `b`. For instance `assert(p != &b);`.  
But the programmer will not do that in this code. The expectation is that code naturally will avoid this kind of situation using the returned object in a limited scope.
When objects with static storage duration are returned we don't need to check suspects as well.  This is not implemented yet but one alternative is to use `char * static` to inform *"pointer to static storage duration object"*.

#### Deleting Owner Pointers

**Owner pointers** take on the responsibility of owning the pointed object and its associated storage, treating them as distinct entities.

A common practice is to implement a delete function to release both resources.

**Sample - Implementing the delete function**

```c
#pragma safety enable

#include <stdlib.h>

struct X {
  char * _Owner text; 
};

void x_destroy(_Opt struct X * _Obj_owner x) { 
  free(x->text); 
}

void x_delete(_Opt struct X * _Owner _Opt p) { 
  if (p) {
    x_destroy(p); /* *p is moved*/
    free(p);
  }
}

int main() {
   _Opt struct X * _Owner _Opt pX = calloc(1, sizeof * pX);
   if (pX) {
     /*...*/;
     x_delete( pX); 
   }
 } 

```

<button onclick="Try(this)">try</button>

The differentiation of object and storage ownership is given by the `void *` conversion.  Moving `void*` implies that only the storage without any object on it is being moved.


#### Conversion from `T * _Owner` to `void * _Owner`

**Rule:** Assignment or cast from `T * _Owner` to `void * _Owner` requires the pointed object T to be empty.

```c  
#pragma safety enable

struct X {
    char * _Owner text;
};

struct X * _Owner make();

int main(){
   void * _Owner _Opt p = nullptr;
   struct X * _Owner pX = make();
   p = pX; //warning
}

```

<button onclick="Try(this)">try</button>

When the object is created on the stack, we can implement a destructor.

**Sample - Implementing a destructor**

```c
#pragma safety enable

#include <stdlib.h>

struct X {
    char * _Owner _Opt text;
};  

void x_destroy(struct X x) {
    free(x.text);
}  

int main() {
    struct X x = {};
    /*...*/
    x_destroy(x); /*x is moved*/
}
```
<button onclick="Try(this)">try</button>

However in C, structs are typically passed by pointer rather than by value. To transfer the ownership of an owner object to a pointer, Cake introduces a new qualifier, **\_Obj\_owner**. 
  
It can also be seen as an owner pointer that owns the object but not the storage. (Better names?)

A pointer qualified with **\_Obj\_owner** is the owner of the pointed object but not responsible for managing its memory.

The next sample illustrates how to implement a destructor using a \_Obj\_owner pointer parameter.

**Sample - Implementing a destructor using \_Obj\_owner**

```c
#pragma safety enable

#include <stdlib.h>

struct X {
    char * _Owner _Opt text;
};

void x_destroy(_Opt struct X * _Obj_owner x) {
    free(x->text);
    /*x is not the owner of the memory*/
}

int main() {
    struct X x = {};
    /*more code*/
    x_destroy(&x); /*x is moved*/
}
```

<button onclick="Try(this)">try</button>


In order to prevent moving from a non owner object, only _address of expressions_ to **\_Obj\_owner** are allowed. 

**Sample - Non address of expression or owner pointer.**

```c
#pragma safety enable

#include <stdlib.h>

struct X {
 struct Y * p;
};

void y_destroy(struct Y * _Obj_owner p);

void f(struct X * x) {
  //Error: parameter 1 requires a pointer to owner object
  //Error: pointed object is not owner
  y_destroy(x->p); 
}
```
 
 <button onclick="Try(this)">try</button>

We can copy an **owner** pointer to an **\_Obj\_owner** pointer. In this scenario, only the ownership of the pointed object is transferred, not the memory ownership.   

**Sample - Using `x_destroy` to implement `x_delete`**

```c
#pragma safety enable

#include <stdlib.h>

struct X {
  char * _Owner text; 
};

void x_destroy(_Opt struct X * _Obj_owner x) { 
  free(x->text); 
}

void x_delete(_Opt struct X * _Owner _Opt p) { 
  if (p) {
    x_destroy(p); /* *p is moved*/
    free(p);
  }
}

int main() {
   struct X * _Opt _Owner pX = calloc(1, sizeof * pX);
   if (pX) {
     /*...*/;
     x_delete(pX); 
   }
 } 

```

<button onclick="Try(this)">try</button>

In C, array types in arguments are pointers. This characteristics is preserved.

To use owner qualifier in array we do. (Just like const)

```c
#pragma safety enable

void free(void * _Owner _Opt p);

void f(int a[_Owner])
{
  free(a);
}
```

<button onclick="Try(this)">try</button>

But I think this is quite uncommon.


## Flow analysis  

Flow analysis is the core feature that enables the nullable and lifetime checks.
  
The compiler flag `-fanalyzer` activates the flow analysis that works a secondary pass.

Flow analysis  also can be enabled/disable with pragma

```c
#pragma flow enable
```

When pragma safety, nullable or ownership are enabled, they enable flow as well.

To check the nullable and ownership rules, the compiler use these states:

- nothing
- uninitialized
- moved
- null
- not-null
- zero
- not-zero
- lifetime-ended
 
We can print these states using the **static_debug** declaration.
We can also assert the variable is at a certain state using the **static_state** declaration. 

**Sample - Usage of static\_state and static\_debug**

```c
#pragma safety enable

int main() {
  int a;   
  static_state(a, "uninitialized"); //checks a state  
  static_debug(a);                  //prints 'a' state 
}
```  

<button onclick="Try(this)">try</button>
 
#### Uninitialized state

The **uninitialized** state is the state of local variables that are declared but not initialized. 

Objects are defined as having both a real part and an imaginary one. The real part corresponds to the actual value stored in memory, while the imaginary part lacks any physical representation. Consequently, the uninitialized state refers to this imaginary part, leaving the real value unspecified.

Flow analysis must  ensure that we don't read uninitialized objects.

```c
#pragma safety enable

int printf(const char* restrict format, ...);

void f(int condition) {
   int i;
   if (condition) 
    i = 0;
   printf("%d", i); //warning i may be uninitialized
}
```
<button onclick="Try(this)">try</button>

The other situation were variables becomes `**uninitialized** is when moving ownership to function parameters. 

This prevents bugs like double free or use after free.

```c
#pragma safety enable

int * _Owner f();
void free(void * _Owner _Opt p);

int main() {   
   int * _Owner p = f();
   free(p);
   free(p); //warning p is uninitialized
}
```
<button onclick="Try(this)">try</button>

 
#### Moved state

The **moved** state is similar to the *uninitialized* state, it is also an imaginary state and does not have a representation on storage.  

The difference is that the moved state is used when moving local variables. For pointers, the moved state implies that the pointer was not-null. 
  
**Sample - local scope moves**

```c
#pragma safety enable

int * _Owner f();
void free(void * _Owner _Opt p);

int main() {   
   int * _Owner p = f();
   int * _Owner _Opt p2 = 0;
   p2 = p; // p moved to p2  
  
   //compiler knows that *p still valid  

   free(p); //warning p was moved

   free(p2); //ok
}
```
  <button onclick="Try(this)">try</button>


#### _Out qualifier

A common scenario where uninitialized objects are utilized is when a pointer to an uninitialized object is passed to an "init" function.

This situation is addressed by the qualifier **_Out**.

```c  
#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
  char * _Owner _Opt text;
};

int init(_Out struct X *p)
{
  p->text = strdup("a"); //safe
}

int main() {   
  struct X x;
  init(&x);  
  free(x.text);
}  

```

<button onclick="Try(this)">try</button>

With the \_Out qualifier, caller is informed that the argument must be uninitialized.

The implementation is aware that it can safely override the contents of the object `p->text` without causing a memory leak.

> Note: There is no explicit "initialized" state. When referring to initialized objects, it means the state is neither "moved" nor "uninitialized.".

**Rule:** All objects passed as arguments must be initialized and all objects reachable must be initialized.

**Rule:** By default, the parameters of a function are considered initialized. The exception is created with \_Out qualifier.

**Rule:** We cannot pass initialized objects, or reachable initialized objects to **\_Out** qualified object.

For instance, at set implementation we need free text before assignment.

```c
#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
  char * _Owner _Opt text;
};

int init(_Out struct X *p, const char * text)
{
   p->text = strdup(text); //safe
}

int set(struct X *p, const char * text)
{
  free(p->text); //necessary
  p->text = strdup(text);
}

int main() {   
  struct X x;
  init(&x, "text1");
  set(&x, "text2");   
  free(x.text);
}
```
<button onclick="Try(this)">try</button>

**Rule:** Function never returns uninitialized objects or reachable uninitialized objects.

TODO void objects.

**Rule:** Non owner objects accessible with parameters cannot leave scope with uninitialized/moved objects.

```c  
#pragma safety enable

#include <string.h>
#include <stdlib.h>

struct X {
  char * _Owner _Opt_ name;
};

void x_destroy(struct X * _Obj_owner p) {
  free(p->name); 
}

struct Y {
   struct X x;
}

void f(struct Y * p) {   
   x_destroy(&p->x); //breaking the rule
}
  
int main() {
   struct Y  y = {};
   y.x.name = strdup("a");
   f(&y);
   free(y.x.name);
}  

```
  
  <button onclick="Try(this)">try</button>

Sample of swap if fine because at end of scopes objects are not uninitialized/moved.

```c  
#pragma safety enable

#include <stdlib.h>

struct X
{
  char * _Owner name;
};

void x_destroy(struct X * _Obj_owner p)
{
  free(p->name); 
}

void x_swap(struct X * a, struct X * b) {
  struct X temp = *a;
  *a = *b;
  *b = temp;
} //ok
  
int main() {
   struct X x1 = {};
   struct X x2 = {};
  
   x_swap(&x1, &x2);
  
   x_destroy(&x1);
   x_destroy(&x2);
}
```


<button onclick="Try(this)">try</button>

#### Null and Not-Null state

The **null** state means that pointers/objects are empty, In other words, not referencing any object. 

**Sample - Null state**

```c
#pragma safety enable

int main() {
 void * _Owner _Opt p = nullptr;   
 static_state(p, "null"); 
}
```  
<button onclick="Try(this)">try</button>

**Rule:** Before assignment, owner objects, must be empty or uninitialized.
The state must be null or uninitialized/moved.
  
Sample

```c  
#pragma safety enable

#include <stdio.h>
int main() {
  FILE * _Owner _Opt file = fopen("file.txt", "r");
  file = fopen("file.txt", "r"); //warning
}
```


<button onclick="Try(this)">try</button>

The **not-null** state indicates that the object is initialized and referencing an object.

The final state is combination of possibilities like **null** and **not-null**. 
We can check possible combinations using "|" at `static_state`.  

```c
#pragma safety enable

#include <stdlib.h>

int main()
{
   void * _Owner p = malloc(1);
   if (p) {
     static_state(p, "not-null");
   }      
   static_state(p, "null | not-null");
   free(p);
}
```
<button onclick="Try(this)">try</button>


#### Zero and Not-Zero state

The **zero** state is used for non-pointer objects to complement and support uninitialized checks.

**Sample - The zero state**

```c
int main()
{
   int i = 0;
   static_state(i, "zero");   
}
```

<button onclick="Try(this)">try</button>


**Zero** and **null** are different states.
This difference is necessary because, for non-pointers like the socket sample, 
0 does not necessarily means null. 
The compiler does not know the semantics for types that are not pointers.

#### lifetime-ended state

This is the state when variables leave the scope or when objects are moved.


#### static_set

We can use **static_set** to override states.
In the next sample, we annotate that server_socket is null, which doesn't mean it 
is zero but indicates that it is not holding any resources and 
is safe to return without calling close.

**Sample - Usage of static_set**

```c
  _Owner int server_socket =
     socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {  
     static_set(server_socket, "null");
     return;
  }  
 /*...*/
 close(server_socket);
```

The **not-zero** state is used in non-pointers objects to indicate the value is not zero.


```c
int f();

int main() {   
    int i = f();
    static_state(i, "zero | not-zero");
}
```

<button onclick="Try(this)">try</button>

  
Now let's consider `realloc` function.

```c
void * _Owner _Opt _realloc( void * _Opt ptr, size_t new_size );	
```

In the declaration of `realloc`, we are not moving the ptr. The reason for that is because the `ptr` may or may not be moved. If the function returns NULL, `ptr` was not moved. 

**Sample - Using static_set with realloc**

```c
#pragma safety enable

void* _Owner _Opt malloc(size_t size);
void* _Owner _Opt realloc(void* _Opt ptr, size_t size);

int main()
{
  void * _Owner _Opt p = malloc(1);
  void * _Owner _Opt p2 = realloc(p, 2);
  if (p2 != 0)
  {
     // if p2 != 0 it  means p was moved
     static_set(p, "moved");
     p = p2;
  }    
  free(p);
}

```
<button onclick="Try(this)">try</button>

Note: A contract syntax is being considered to remove the assert from the caller side and move it to a function contract declaration. 

### assert is a built-in function

Consider the following sample where we have a linked list. 
Each node has owner pointer to next. 
The next pointer of the tail of the list is always pointing to null, unless we have a bug. But the compiler does not know `list->tail->next` is null. Using assert we give this inform to the compiler and we also have a runtime check for possible logic bugs.

**Listing 22 shows the usage of assert.** 

```c
#pragma safety enable

#include <string.h>
#include <stdlib.h>

struct node {
 char * _Owner text;
 struct node* _Owner next;
};

struct list {
  struct node * _Owner head;
  struct node * tail;
};

void list_append(struct list* list, struct node* _Owner node)
{
  if (list->head == NULL) 
  {
     list->head = node;
  }
  else
  {      

     //list->tail is not null. (becuase it is only null if head is also null)
     assert(list->tail != 0);         

     //next is always null becuase it is the last node
     assert(list->tail->next == 0);

     list->tail->next = node;
   }
   list->tail = node;
}
```
  <button onclick="Try(this)">try</button>


## Code transition Strategy

If the compiler supports ownership checks it must define  `__STDC_OWNERSHIP__`. 

If the compiler supports nullable types it must define  `__STDC_NULLABLE__`. 

If the compiler supports flow analysis it must define  `__STDC_FLOW__`. 


A header like this `safe.h` can be created.

```c

#ifdef  __STDC_FLOW__
 #pragma flow enable
#endif

#ifdef  __STDC_OWNERSHIP__
 #pragma ownership enable
#else
  /*empty macros*/

  #define _Out
  #define _Owner
  #define _Obj_owner
  #define _View

#endif

#ifdef  __STDC_NULLABLE__
 #pragma nullable enable
  #pragma flow enable
#else
  /*empty macros*/
  #define _Opt
#endif

```

```c
#include <safe.h>
int main()
{
}
```


## Glossary

#### undefined behavior (From C23)
behavior, upon use of a nonportable or erroneous program construct or of erroneous data, for which
this document imposes no requirements
2 Note 1 to entry: Possible undefined behavior ranges from ignoring the situation completely with unpredictable results,
to behaving during translation or program execution in a documented manner characteristic of the environment (with or
without the issuance of a diagnostic message), to terminating a translation or execution (with the issuance of a diagnostic
message).

#### indeterminate representation (From C23)
object representation that either represents an unspecified value or is a non-value representation  

#### unspecified value (From C23)
valid value of the relevant type where this document imposes no requirements on which value is  

#### unspecified behavior (From C23)
behavior, that results from the use of an unspecified value, or other behavior upon which this
document provides two or more possibilities and imposes no further requirements on which is
chosen in any instance

#### lifetime (From C23)
The lifetime of an object is the portion of program execution during which storage is guaranteed
to be reserved for it. An object exists, has a constant address36), and retains its last-stored value
throughout its lifetime37) If an object is referred to outside of its lifetime, the behavior is undefined.
If a pointer value is used in an evaluation after the object the pointer points to (or just past) reaches
the end of its lifetime, the behavior is undefined. The representation of a pointer object becomes
indeterminate when the object the pointer points to (or just past) reaches the end of its lifetime

#### object (From C23)
region of data storage in the execution environment, the contents of which can represent values


## References

[1] https://learn.microsoft.com/en-us/dotnet/csharp/nullable-references, https://learn.microsoft.com/en-us/dotnet/csharp/nullable-migration-strategies?source=recommendations

