  
Last Updated 22/03/2024
  
This is a work in progress, both design and implementation. Cake source itself is being used to validate the concepts.

## Abstract
The objective is to statically check code and prevent bugs, including memory bugs. For this task, the compiler needs information that humans typically gather from the context. For example, names like "destroy" or "init" serve as hints, along with documentation and sometimes the implementation itself.

The compiler doesn't read documentation, nor does it operate in the same way as humans. Instead, a formal means of communication with the compiler is necessary. To facilitate this, new qualifiers have been created, and new methods of communication with the compiler have been established.

In the end, we still have the same language, but with a c\_type\_system++ version of C. This c\_type\_system++ can be disabled, and the language remains unmodified.

The creation of these rules follows certain principles, one of which is to default to safety.
In cases of uncertainty, the compiler should seek clarification. While C programmers retain the freedom 
to code as they wish, they must either persuade the compiler or disable analysis in specific code sections.
A human factor must be considered to ensure that annotations do not make the work too boring with 
excessive details. In this regard, selecting defaults that cover the most common cases is crucial.

## Concepts

### Owner Objects

An **owner object** is an object referencing another object and managing its lifetime. 

The most common type of owner objects are pointers, often referred as **owner pointers**. An owner pointer is created with the qualifier owner.

**Sample - Owner Pointer to FILE**

```c
#include <ownership.h>
#include <stdio.h>

int main()
{
  FILE *owner f = fopen("file.txt", "r"); 
  if (f)
    fclose(f);
}
```

> Note: **owner** is actually a macro declared in <ownership.h> as **_Owner**. 

The ownership mechanism has some rules that will be listed gradually throughout the text.

**Rule:** An **owner object** is always the unique owner of the referenced object.

**Rule:** When owner objects are copied the ownership is transferred.

**Rule:** Before the end of its lifetime, owner objects must move the ownership of the objects they own.


Sample

```c  
#include <ownership.h>
#include <stdio.h>

int main()
{
	FILE *owner f = fopen("file.txt", "r");
	FILE *owner f2 = f; /*MOVED*/
	if (f2)
       fclose(f2); /*MOVED*/
}
```

Invoking a function `fclose` is analogous to assignment of the argument `f2`, resulting in the transfer of ownership of `f2` to the function parameter.  

Sample - Declaration of fopen and fclose

```c
FILE * _Owner _fopen( const char *filename, const char *mode );
void fclose(FILE * _Owner p);
```

> Note: The cake ownership model does not include the concept of a destroyed or deleted object. Instead, everything is viewed as a transformation, where the object is broken into smaller parts and those parts are moved.

### Non-pointer owner objects

We can have other types of **owner objects**. For instance, Berkeley sockets use an integer to identify the socket.

Sample

```c
 owner int server_socket =
     socket(AF_INET, SOCK_STREAM, 0);
 /*...*/
 close(server_socket);
```

> Note: The location and usage of the qualifier owner is similar to the const qualifier. For pointers, it goes after *, and for this socket sample, it can be before int. The owner qualifier belongs to the object (memory) that holds the reference.

When a struct or union have at least one owner object it makes the struct a owner object too.

**Rule:** Owner objects cannot be discarded.

```c  
#include <ownership.h> 
#include <stdio.h>

int main() {  
  fopen("file.txt", "r"); //warning   
}
```

**Rule:** A non-owner object cannot be copied to a owner object.

**Rule:** The null pointer constant can be used to initialize owner objects. (Even it its type is non-owner)

**Sample**

```c
FILE * f(); //returning non owner
int main() {  
   FILE * owner file = f(); //ERROR   
   FILE * owner file2 = 0;  //OK
}
```


### View Objects

A **view object** is an object referencing another object without managing its lifetime. 

**Rule:** The lifetime of the referenced object must be longer than the lifetime of the view object.

The most common view objects are pointers called **view pointers**. 

The view qualifier is not necessary for pointers, since it's the default behavior. When an owner object is copied to a view object, the ownership is not transferred.
  
**Sample**

```c
#include <ownership.h>
#include <stdio.h>

void use_file(FILE *f) {}

int main() {
    FILE *owner f = fopen("file.txt", "r");
    if (f) {
        use_file(f); /*NOT MOVED*/
        fclose(f);
    }
}
```



When a **view** qualifier is used in structs, it makes all members as view objects. 
 
**Sample - A view parameter**

```c
#include <ownership.h>
#include <stdlib.h>

struct X {   
  char *owner text;   
};  

void f(view struct X x) { /*...*/ }  

int main() {
    struct X x = {};
    f(x); /*NOT MOVED*/    
    free(x.text);
}
```

> Note: It is interesting to compare against const qualifier. While const adds a qualifier "const" "view" removes the qualifier "owner".

### Returning a pointer to a view object
  
We can check the rule "The lifetime of the referenced object must be longer than the lifetime of the view object" with these constrains.

We cannot return the address of local variables

```c
int * f()
{
   int a = 1;
   return &a; //ERROR
}
```  
  
We can return the address of global variables

```c
static int a = 1;
int * f()
{   
   return &a; // OK
}  
```  
  
And we can return parameters

```c
int * f2(int *p) {
   return p; //OK
}
```

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

Examining the implementation reveals that the returned view pointer's lifetime can be that of either 'a' or 'b'.

Our goal is to set contracts at the declaration level.

Following the concept of ensuring safety by default, we assume that the returned view pointers have the shortest scope, limited to the function call.  

> Note : Currently, this check is missing at cake 

### View pointer as struct members
Consider this sample.

```c  
struct X {  
 struct Y * pY;  
};  
struct Y {  
 char * owner name;  
};  
```

The rule "The lifetime of the referenced object must be longer than the lifetime of the view object" needs to be checked at each instantiation.

> Note : Currently, this check is missing at cake 

### Deleting Owner Pointers

**Owner pointers** take on the responsibility of owning the pointed object and its associated memory, treating them as distinct entities. A common practice is to implement a delete function to release both resources.

**Sample - Implementing the delete function**

```c
#include <ownership.h>
#include <stdlib.h>

struct X { 
  char *owner text; 
};

void x_delete(struct X *owner p) {
  if (p) {
    /*releasing the object*/ 
    free(p->text);
    
    /*releasing the memory*/ 
    free(p); 
  }
}

int main() {
  struct X * owner pX = calloc(1, sizeof * pX);
  if (pX) {
    /*...*/;
    x_delete( pX); 
  }
}
```

### Conversion from owner pointer to void * owner

**Rule:** Assignment or cast of a owner pointer to void * owner requires the pointed object to be empty.

```c  
struct X {
    char _Owner text;
};

struct X * _Owner make();

int main(){
   void * _Owner p = nullptr;
   _Owner auto pX = make();
   p = pX; //warning
}
```

When the object is created on the stack, we can implement a destructor.

**Sample - Implementing a destructor**

```c
#include <ownership.h>
#include <stdlib.h>

struct X {
    char *owner text;
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

However in C, structs are typically passed by pointer rather than by value. To transfer the ownership of an owner object to a pointer, Cake introduces a new qualifier, **obj_owner**. 

A pointer qualified with **obj_owner** is the owner of the pointed object but not responsible for managing its memory.

The next sample illustrates how to implement a destructor using a obj_owner pointer parameter.

**Sample - Implementing a destructor using obj_owner**

```c
#include <ownership.h>
#include <stdlib.h>

struct X {
    char *owner text;
};

void x_destroy(struct X *obj_owner x) {
    free(x->text);
    /*x is not the owner of the memory*/
}

int main() {
    struct X x = {};
    /*more code*/
    x_destroy(&x); /*x is moved*/
}
```

In order to prevent moving from a non owner object, only _address of expressions_ to **obj_owner** are allowed. 

**Sample - Non address of expression or owner pointer.**

```c
#include <ownership.h>
#include <stdlib.h>

struct X {
 struct Y * p;
};

void y_destroy(struct Y * obj_owner p);

void f(struct X * x) {
  //Error: parameter 1 requires a pointer to owner object
  //Error: pointed object is not owner
  y_destroy(x->p); 
}
```
 
We can copy an **owner** pointer to an **obj_owner** pointer. In this scenario, only the ownership of the pointed object is transferred, not the memory ownership.   

**Sample - Using `x_destroy` to implement `x_delete`**

```c
#include <ownership.h>
#include <stdlib.h>

struct X {
  char *owner text; 
};

void x_destroy(struct X *obj_owner x) {
  free(x->text); 
}

void x_delete(struct X *owner p) { 
  if (p) {
    x_destroy(p); /* *p is moved*/
    free(p);
  }
}

int main() {
   struct X * owner pX = calloc(1, sizeof * pX);
   if (pX) {
     /*...*/;
     x_delete( pX); 
   }
 } 
```

In C, array types in arguments are pointers. This characteristics is preserved.

To use owner qualifier in array we do. (Just like const)

```c
void f(int a[owner])
{
  free(a);
}
```

But I think this is quite uncommon.


## Flow analysis  

Let's revisit our first example:

```c
#include <ownership.h>
#include <stdio.h>

int main()
{
  FILE *owner f = fopen("file.txt", "r"); 
  if (f)
    fclose(f); //f is moved
}
```

"Rule: Before the end of its lifetime, owner objects must move the ownership of the objects they own."

At the end of the scope, `f` can be either null or moved, and both states ensure that the rule is followed.

To check the ownership rules, the compiler need flow analysis and it uses six states:

- uninitialized
- moved
- null
- not-null
- zero
- not-zero
 
We can print these states using the **static_debug** declaration. We can also assert the variable is at a certain state using the **static_state** declaration. 

**Sample - Usage of static\_state and static\_debug**

```c
int main() {
 int a;   
 static_state(a, "uninitialized"); //checks a state  
 static_debug(a);                  //prints 'a' state 
}
```  
  
Output:

```
c:/main.c:4:2: note: static_debug
 4 | static_debug(a);
   | ^~~~~~~~~~~~
 a == "uninitialized"
```
 
#### Uninitialized state

The **uninitialized** state is the state of variables that are declared but not initialized. 

Flow analysis must  ensure that we don't read uninitialized objects.

```c
void f(int condition) {
   int i;
   if (condition) 
    i = 0;
   printf("%d", i); //warning i may be uninitialized
}
```

The other situation were variables becomes **uninitialized** is when moving ownership to function parameters. 

This prevents bugs like double free or use after free.

```c
int * _Owner f();
void free(void * _Owner _Opt p);

int main() {   
   int * _Owner p = f();
   free(p);
   free(p); //warning p is uninitialized
}
```

#### Moved state

The **moved** state is similar to the uninitialized state. The difference is that the moved state is used when moving local variables. This information could be useful in the future to be less restrictive than uninitialized.
  
 **Sample - local scope moves**

```c
int * _Owner f();
void free(void * _Owner _Opt p);

int main() {   
   int * _Owner p = f();
   int * _Owner p2 = 0;
   p2 = p; // p moved to p2  
  
   //compiler knows that *p still valid  

   free(p); //warning p was moved
   free(p2); //ok
}
```
  

#### _Out qualifier

A common scenario where uninitialized objects are utilized is when a pointer to an uninitialized object is passed to an "init" function.

This situation is addressed by the qualifier **_Out/out**.

```c  
#include <ownership.h> 
#include <stdlib.h>
#include <string.h>

struct X {
  char * owner text;
};

int init(out struct X *p)
{
  p->text = strdup("a"); //safe
}

int main() {   
  struct X x;
  init(&x);  
  free(x.text);
}  
```


With the out qualifier, caller is informed that the argument must be uninitialized.

The implementation is aware that it can safely override the contents of the object `p->text` without causing a memory leak.

> Note: There is no explicit "initialized" state. When referring to initialized objects, it means the state is neither "moved" nor "uninitialized.".

**Rule:** All objects passed as arguments must be initialized and all objects reachable must be initialized.
  

**Rule:** By default, the parameters of a function are considered initialized. The exception is created with out qualifier.

**Rule:** We cannot pass initialized objects, or reachable initialized objects to **out** qualified object.

For instance, at set implementation we need free text before assignment.

```c
#include <ownership.h> 
#include <stdlib.h>
#include <string.h>

struct X {
  char * owner text;
};

int init(out struct X *p, const char * text)
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

**Rule:** Function never returns uninitialized objects or reachable uninitialized objects.

TODO void objects.

**Rule:** Non owner objects accessible with parameters cannot leave scope with uninitialized/moved objects.

```c  
#include <ownership.h> 
#include <string.h>
#include <stdlib.h>

struct X {
  char * owner name;
};

void x_destroy(struct X * obj_owner p) {
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
  
Sample of swap if fine because at end of scopes objects are not uninitialized/moved.

```c  
#include <ownership.h> 
#include <stdlib.h>

struct X
{
  char * owner name;
};

void x_destroy(struct X * obj_owner p)
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

#### Null and Not-Null state

The **null** state means that pointers/objects are initialized and not referencing any object. 

**Sample - Null state**

```c
#include <ownership.h> 

int main() {
 void * owner p = nullptr;   
 static_state(p, "null"); 
}
```  

**Rule:** Before assignment, owner objects, must not be holding objects. The state must be null or uninitialized/moved.
  
Sample

```c  
#include <ownership.h> 
#include <stdio.h>
int main() {
  FILE * owner file = fopen("file.txt", "r");
  file = fopen("file.txt", "r"); //warning
}
```

The **not-null** state indicates that the object is initialized and not referencing any object.

The final state is combination of possibilities like **null** and **not-null**. We can check possible combination using "or" at `static_state`.  

The combination **null or not-null** has a alias **maybe-null**.

**Sample - not-null and maybe-null**

```c
#include <ownership.h> 
#include <stdlib.h>

int main()
{
   void * owner p = malloc(1);
   if (p) {
     static_state(p, "not-null");
   }
   static_state(p, "maybe-null");   
   static_state(p, "null or not-null");
   free(p);
}
```


**Rule:** Pointer parameters are consider not-null by default. The exception is created using the qualifier  **_Opt**.

The  **_Opt**. qualifier is used to tell the caller that the pointer can be at null state and tells the implementation that it is necessary to check the pointer for null before usage. 

**Rule:** Pointers that can be null cannot be passed to functions that don't declare the pointer as opt.

```c
#include <ownership.h> 
#include <stdlib.h>

struct X {int i;};

void f(struct X * p){
}

int main()
{
   struct X * owner p = calloc(1, sizeof * p);
   f(p); //warning pointer can be null
   free(p);
}
```

> Note: To enable null checks in cake use -nullchecks
  
This sample can be fixed in two ways.  

```c
/*...*/
void f(struct X * p){}
int main()
{
   struct X * owner p = calloc(1, sizeof * p);
   if (p)   
     f(p); //ok
   free(p);
}
```

or 

```c
/*...*/
void f(struct X * opt p){ }
int main()
{
   struct X * owner p = calloc(1, sizeof * p);
   f(p); //ok
   free(p);
}
```

Considering these semantics, the correct declaration of free is:

```c
void free(void * _Owner _Opt p); //p can be null 
```


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

**Zero** and **null** are different states. This difference is necessary because, for non-pointers like the socket sample, 0 does not necessarily means null. The compiler does not know the semantics for types that are not pointers.

#### static_set

We can use **static_set** to override states. In the next sample, we annotate that server_socket is null, which doesn't mean it is zero but indicates that it is not holding any resources and is safe to return without calling close.

**Sample - Usage of static_set**

```c
  owner int server_socket =
     socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {  
     static_set(server_socket, "null");
     return;
  }  
 /*...*/
 close(server_socket);
```

The **not-zero** state is used in non-pointers objects to indicate the value is not zero.

**any** is a alias for **zero or not-zero** state.

```c
int f();

int main() {   
    int i = f();
    static_state(i, "any");
}
```

  
Now let's consider `realloc` function.

```c
void * owner realloc( void *ptr, size_t new_size );	
```

In the declaration of `realloc`, we are not moving the ptr. The reason for that is because the `ptr` may or may not be moved. If the function returns NULL, `ptr` was not moved. 

**Sample - Using static_set with realloc**

```c
#include <ownership.h> 
#include <stdlib.h>
int main()
{
  void * owner p = malloc(1);
  void * owner p2 = realloc(p, 2);
  if (p2 != 0)
  {
     // if p2 != 0 it  means p was moved
     static_set(p, "moved");
     p = p2;
  }    
  free(p);
}
```


### assert is a statement

When possible we can use assert that works both as static information and runtime check in debug.

Consider the following sample where we have a linked list. Each node has owner pointer to next. The next pointer of the tail of the list is always pointing to null, unless we have a bug. But the compiler does not know `list->tail->next` is null. Using assert we give this inform to the compiler and we also have a runtime check for possible logic bugs.

**Listing 22 shows the usage of assert.** 

```c

#include <ownership.h> 
#include <string.h>
#include <stdlib.h>

struct node {
 char * owner text;
 struct node* owner next;
};

struct list {
  struct node * owner head;
  struct node * tail;
};

void list_append(struct list* list, struct node* owner node)
{
  if (list->head == NULL) {
      list->head = node;
   }
   else {
      assert(list->tail->next == 0);
      list->tail->next = node;
   }
   list->tail = node;
}
```
  

## Ownership Feature Strategy (Inspired by stdbool.h)

If the compiler supports ownership checks and qualifiers such as _Owner, _View, _Obj\_view, etc., it must define  `__STDC_OWNERSHIP__`. 

We utilize a header with macro versions for each keyword. 
For example, the macro owner corresponds to the keyword _Owner. 
This approach offers the advantage that macros can be declared empty for compilers that 
do not support ownership checks.

```c
#include <ownership.h>
#include <stdlib.h>

int main() {
  void * owner p = malloc(1); //error: missing owner qualifier
}
```

> Currently cake is using the same headers of VS and GCC that are not aware of ownership. 
For this reason, `ownership.h` itself is declaring malloc.
When we parsing malloc from MSVC/GCC we ignore the diferences, but at the current version 
`ownership.h` header must be included before to take efect.


The standard also could make out, opt, view, owner, obj_owner as reserved keyword for the future.

_