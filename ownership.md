  
Last Updated 10/12/2023

## Owner Objects

An **owner object** is an object referencing another object and managing its lifetime.

The most common type of owner objects are pointers, often referred as **owner pointers**. An owner pointer is created with the qualifier owner, as illustrated in Listing 1:

**Listing 1 - Owner Pointer to FILE**

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

**Note:** The `owner` is actually a macro declared in ownership as _Owner. 

The ownership mechanism has some rules that will be listed gradually throughout the text.

**Rule**: An **owner object** is always the unique owner of the referenced object.

**Rule**: When owner objects are copied the ownership is transfered.

**Rule:** Before the end of its lifetime, owner objects must move the ownership of the objects they own. 

For example, in Listing 2, the ownership of the owner pointer `f` is transferred to `f2`:

**Listing 2 - Assignment of Owner Objects is a Move**

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

Invoking a function `fclose` is analogous to assignment of the argument `f2`, resulting in the transfer of ownership of `f2` to the function parameter.  Listing 3, shows the declaration of `fclose`.

**Listing 3 - Declaration of close**

```c
void fclose(FILE *owner p);
```

### Non-pointer owner objects

We can have other types of **owner objects**. For instance, Berkeley sockets use an integer to identify the socket, as shown in listing 4:

**Listing 4 - Non-Pointer owners objects**

```c
 owner int server_socket =
     socket(AF_INET, SOCK_STREAM, 0);
 /*...*/
 close(server_socket);
```

The location and usage of qualifier owner is similar of const qualifier. For pointers it goes after `*`, for this socket sample it can be before int.

## View Objects

A **view object** is an object referencing another object without managing its lifetime. 

**Rule** The lifetime of the referenced object must be bigger than the lifetime of the view object.

The most common view objects are pointers called **view pointers**. 

The view qualifier is not necessary for pointers, since it's the default behavior. When an owner object is copied to a view object, the ownership is not transferred, as shown in Listing 5:

**Listing 5 - Calling Function with View Parameters**

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

When a struct or union have at least one owner object it makes the struct a owner object too.

When a **view** qualifier is used in structs, it makes all members as view objects. Listing 6.
  


**Listing 6 - A view parameter**

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


## Deleting Owner Pointers

**Owner pointers** take on the responsibility of owning the pointed object and its associated memory, treating them as distinct entities. A common practice is to implement a delete function to release both resources, as illustrated in Listing 7:

**Listing 7 - Implementing the delete function**

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
  struct X * owner pX = calloc( 1, sizeof * pX);
  if (pX) {
    /*...*/;
    x_delete( pX); 
  }
}
```

When the object is created on the stack, we can implement a destructor, as shown in Listing 8:

**Listing 8 - Implementing a destructor**

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

Listing 9 illustrates how to implement a destructor using a obj_owner pointer parameter.

**Listing 9 - Implementing a destructor using obj_owner**

```c
struct X {
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

In order to prevent moving from a non owner object, only `address of expressions` to `obj_owner` are allowed. For instance, listing 10 shows we cannot move a view pointer.

**Listing 10 - Non address of expression or owner pointer.**

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

## Copying a owner pointer to a obj_owner pointer
 
We can copy an owner pointer to an **obj_owner** pointer. In this scenario, only the ownership of the pointed object is transferred, not the memory ownership. Listing 11 illustrates how we can use `x_destroy` in the implementation of `x_delete`.

**Listing 11 - Using `x_destroy` to implement `x_delete`**

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
   struct X * owner pX = calloc( 1, sizeof * pX);
   if (pX) {
     /*...*/;
     x_delete( pX); 
   }
 } 
```


## Static analysis - Checking the rules at compile time  

Let's revisit our first example:

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

The flow analysis must ensure that when the owner pointer `f` goes out of scope, it does not own any objects. At the end of the scope, `f` can be either null or moved, and both states ensure that there are no remaining resources.

To check the ownership rules, the compiler uses six states:

- uninitialized
- moved
- null
- not-null
- zero
- not-zero
 
We can print these states using the **static_debug** declaration. We can also assert the variable is at a certain state using the **static_state** declaration. Listing 12 shows this usage:

**Listing 12 - Usage of **static_state** and **static_debug****

```c
int main() {
 int a;   
 static_state(a, "uninitialized");  
 static_debug(a);
}
```  
  
Output:

```
c:/main.c:4:2: note: static_debug
 4 | static_debug(a);
   | ^~~~~~~~~~~~
 a == "uninitialized"
```
  
As we have just seen, the **uninitialized** state is the state of variables that are declared but not initialized. The compiler ensures that we don't read uninitialized objects.

The **null** state means that owner objects are initialized and not referencing any object. Listing 13 shows a sample using owner pointers:

**Listing 13 - Null state**

```c
#include <ownership.h> 

int main() {
 void * owner p = nullptr;   
 static_state(p, "null"); 
}
```  

The **not-null** state indicates that the object is referencing some object.
The state can be a combination of possibilities like **null** and **not-null**. We can check possible combination using "or".
This particular combination **null or not-null** has a alias **maybe-null** as shown in listing 14.

**Listing 14 - not-null and maybe-null state**

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

The **zero** state is used for non-owner objects to complement and support uninitialized checks.

**Listing 15 - The zero state

```c
int main()
{
   int i = 0;
   static_state(i, "zero");   
}
```

**Zero** and **null** are different states. This difference is necessary because, for non-pointers like the socket sample, 0 does not necessarily means null. The compiler does not know the semantics for types that are not pointers. However, you can use **static_set** to override states. In Listing 16, we annotate that server_socket is null, which doesn't mean it is zero but indicates that it is not holding any resources and is safe to return without calling close.

The **not-zero** state is used for non-owner objects to indicate the value if not zero.

**Listing 16 - Usage of static_set**

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

Now let's consider `realloc` function.

```c
void * owner realloc( void *ptr, size_t new_size );	
```

In the declaration of `realloc`, we are not moving the ptr. The reason for that is because the `ptr` may or may not be moved. If the function returns NULL, `ptr` was not moved. Listing 17 shows how **static_set** can be used.

**Listing 17 - Using static_set with realloc**

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

Without the `static_set` we have this error   

```
error: memory pointed by 'p' was not released before assignment.
 11 |     p = p2;
```

The state of an object is a combination of all possible states. For instance, let's print and check the state of `f` at listing 17.

**Listing 17 - Flow analysis**

```c  
#include <ownership.h> 
#include <stdlib.h>
#include <stdio.h>

int main() {
  FILE *owner f = fopen("file.txt", "r");
  if (f)
    fclose(f);  
  static_state(f, "uninitialized or null");  
}
```

When objects are moved to functions, the state is  `uninitialized`  that is the worst scenario of what can happens inside the function. When objects are moved inside the same source the state is `moved`.

```
#include <ownership.h> 
#include <stdlib.h>
#include <stdio.h>

int main() {
  FILE *owner f = fopen("file.txt", "r");  
  FILE *owner f2 = f; 
  static_state(f, "moved");  
  fclose(f2);
}  
```


**Check:** We cannot discard owner objects as showed in listing 18.  

**Listing 18 - owner objects cannot be discarded.**

```c  
#include <ownership.h> 
#include <stdio.h>

int main() {
  //error: ignoring the result of owner type
  fopen("file.txt", "r");   
}
```

**Rule:** Before the assignment of owner objects, the compiler checks if the owner object is not holding any resource, as shown in Listing 19:

**Listing 19 - Check before assignment**

```c
#include <ownership.h> 
#include <stdio.h>

int main() {
  FILE * owner file = fopen("file.txt", "r");
  
  //error: memory pointed by 'file' was not 
  //released before assignment.  
  file = fopen("file.txt", "r");
}
```

Sometimes is not possible to infer the state. Consider this sample.

**Listing 20 - Some states cannot be inferred**

```c
#include <ownership.h> 
#include <string.h>

struct X {
  char *owner text; 
};

void init(struct  X * x) {
  //Error: memory pointed by 'x.text' was 
  // not released before assignment
  x->text = strdup("a");
}

```

How to know `x->text` was not holding any resource? At  listing 21, we show how **static_set** can be used to say `x->text` is not initialized at `init`. 

**Listing 21 - Using static_set**

```c
#include <ownership.h> 
#include <string.h>

struct X {
  char *owner text; 
};

void init(struct X * x) {
  static_set(x->text, "uninitialized");
  x->text = strdup("a");
}
```

At this `init` sample we cannot check the uninitialized at runtime because it can be anything.
 
But when possible we can use assert that works both as static information and runtime check in debug.

Consider the following sample where we have a linked list. Each node has owner pointer to next. The next pointer of the tail of the list is always pointing to null, unless we have a bug. But the compiler does not know `list->tail->next` is null. Using assert we give this inform to the compiler and we also have a runtime check for possible logic bugs.

**Listing 22 shows the usage of assert. 

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
  
**Rule:** A non-owner object cannot be copied to a owner object. 

But, the null pointer constant is converted to a null owner pointer. Se listing 23.

**Listing 23 - non owner cannot be copied to owner

```c
FILE * f();
int main() {  
   FILE * owner file = f(); //ERROR   
   FILE * owner file2 = 0;  //OK
}
```
  
**Rule:** A view pointer parameter cannot leave the scope with moved/uninitialized objects. Listing 24

**Listing 24 - Messing with view parameters**

```c

#include <ownership.h> 
#include <string.h>
#include <stdlib.h>

struct X
{
  char * owner name;
};

void x_destroy(struct X * obj_owner p)
{
  free(p->name); 
}

struct Y
{
   struct X x;
}

void f(struct Y * p) {
   // parameter 'p' is leaving scoped with a uninitialized
   // object 'p.x.name'
   x_destroy(&p->x);
}
  
int main() {
   struct Y  y = {};
   y.x.name = strdup("a");
   f(&y);
   free(y.x.name);
}
```

However, listing 25 is correct, because before the end of scope states of the parameters are restored.

**Listing 25 - swap function**

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
}
  
int main() {
   struct X x1 = {};
   struct X x2 = {};
  
   x_swap(&x1, &x2);
  
   x_destroy(&x1);
   x_destroy(&x2);
}
```

**Rule:** When objects are moved to functions, they become uninitialized. This prevents bugs like double free. Listing 26. 

```
 int main() {
  void * owner p = malloc(1);
  free(p);
  free(p); //ERROR p is uninitialized
}
```

**Rule:** moved objects cannot be moved. Listing 27.

**Listing 27 - We cannot move a moved or uninitialized object**

```c
int * owner p1 = ...;
int * owner p2 = p1;
int * owner p3 = p1; //ERROR p1 was moved
```

```c
int * owner f(int * owner p1) {
 int * owner p2 = p1;
 return p1; //ERROR p1 was moved
}
```

**Rule:** When coping a owner object to to a view object the compiler must check the lifetime. Listing 28

**Listing 28 - Lifetime check**

```c
void using_file(FILE * f);

struct X { 
  char *owner text; 
};  
  
struct X * owner make_owner();  

int main() {
  struct X *p = 0;  
  {
    struct X * owner p2 = make_owner();  
    p = p2; //error p lives longer than p2  
    free(p2);
  }

  using_file(fopen("file.txt", "r")); //ERROR
}
```


**Rule:** Returned objects must be valid.
  

**Listing 34 - Assuming returned objects are valid**

```c
void* owner malloc(unsigned long size);
void free(void* owner ptr);

struct X { 
  char *owner text; 
};

struct X * owner f();

int main()
{
   struct X * owner pX = f();
   if (pX) {
     static_state(pX->text, "maybe-null");
   }
}
```


**Rule:** Arguments must be valid.
  

**Listing 35 - Function Arguments cannot be in a moved or uninitialized state**

```c  
#include <ownership.h> 
#include <stdlib.h>
struct X { 
  char *owner text; 
};

void f(struct X * p);

int main() {
   struct X x;
   f(&x); //ERROR
     
   struct X x1 = {0};
   struct X x2 = x1; //MOVED
   f(&x1); //ERROR
}
```

For this reason, an qualifier 'out' can be added in the future to allow passing unitalicized objects.

```c
struct X {
  char *owner text; 
};
void init(out struct  X * x) {  
  x->text = strdup("a");
}
int main() {
  struct X x = {0};
  init(&x);
}
```

Meanwhile, the error can be removed initializing the object.
  
```c
struct X {
  char *owner text; 
};
void init(struct  X * x) {  
  static_set(x, "uninitialized");
  x->text = strdup("a");
}
int main() {
  struct X x = {0};
  init(&x);
}
```

We can also pretend the object is initialized.


```c
struct X {
  char *owner text; 
};
void init(struct  X * x) {  
  static_set(x, "uninitialized");
  x->text = strdup("a");
}
int main() {
  struct X x;
  static_set(x, "zero");
  init(&x);
}
```




## Ownership Feature Strategy (Inspired by stdbool.h)

If the compiler supports ownership checks and qualifiers such as _Owner, _View, _Obj\_view, etc., it must define  `__STDC_OWNERSHIP__`. 

However, even if the compiler implements ownership, it is not active by default. The objective is to have a smooth transition allowing some files without checks. For instance, a thirty part code inside your project.

For instance, when compiling this file, even if the compiler supports ownership we don't have errors or warnings because the checks are not enabled by default.

```c
#include <stdlib.h>

int main() {
  void * p = malloc(1);
}
```

A second define `__OWNERSHIP_H__`, is used to enable ownership.
This define is set when we include `<ownership.h>` at beginning. 

```c
#include <ownership.h>
#include <stdlib.h>

int main() {
  void * p = malloc(1); //error owner required
}
```

The other advantage of having a `<ownership.h>` is because owner is a macro that can be defined as empty in case the compiler does not support ownership, allowing the same code to be compiled in compiler without ownership support. 
  
  
> Currently cake is using the same headers of VS and GCC that are not aware of ownership. For this reason, `ownership.h` itself is declaring malloc etc and the second declaration of malloc inside stdlib.h will not complain with the discrepancy of ownership qualifiers between declarations.

