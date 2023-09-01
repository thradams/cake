

** Under development.**

# Ownership checks for C

The static ownership check is an experimental feature for Cake.
It consists of two separate implementations. 

Part I focuses on introducing the concept of an owner qualifier in the type system and does not requires flow analysis. 

Part II requires flow analysis, ensuring that owned resources are appropriately released when necessary.   
  
By implementing the static ownership check, and using the feature on it's own source, Cake aims to explore and evaluate the effectiveness of this feature.  
  
## Part I - Type system changes  

### New qualifiers

  - owner
  - view
  - obj_owner

The **owner** qualifier declares an object that has a reference to another object.  The owner object exclusively holds the responsibility for controlling the lifetime of the referenced object. When used with a pointer, indicates that the pointer assumes ownership of the pointed object and its associated memory.

```c  
void * owner p = malloc(1);
free(p);
```

The **view** qualifier declares an object that has a reference to another object but it does not control its lifetime, which must exist beyond the lifespan of the view qualified object itself. The **view** qualifier is the default.

```c  
void * owner p = malloc(1);
if (p)
{
  void * p2 = p; /*p2 is a view*/
}
free(p);
```

The **obj\_owner** qualifier is exclusively applicable to pointers, meaning that the pointer owns the pointed object but not the memory it occupies. It is generally used in destructors.    
  
  
```c    
void x_destroy(struct X * obj_owner p){
 /*
    We must release all resources used by p
    but we cannot release the memory pointed by p
 */
}  
int main() {  
  struct X x = {};
  x_destroy(&x);  
}
``` 
 
For struct and unions, if at least one member has the **owner** qualifier, the entire type is considered to be an owner qualified type.

Sample:

```c
struct person {
  char * owner name;
};  

int main(){
  struct person p1 = {};  
  /*p1 is a owner type*/
}
```

Applying the **view** qualifier to an struct or union type designates the type as a view, regardless of whether it includes members owner qualified.

```c
struct person {
  char * owner name;
};

int main(){
  view struct person p2;
   /*p2.name is view qualified*/
}
```

Similarly of what happens with structs and unions, if the element of the array is owner qualified the entire array is treated as an owner qualified type.
  
Sample:

```c
int main()
{
  struct person a[10] = {};  
  //a is the owner of 10 struct person
}
```

When array types are used in parameters the array is not considered owner qualified type. 

```c
void print_array(int n, struct person a[10]);

int main()
{
  struct person a[10] = {};
  print_array(10, a);
}
```

To qualify array parameters as owner we do:

```c
void destroy_array(int n, struct person a[owner 10]);
```

The most common usage of **owner** will be pointers. However, ownership qualifiers can be applied for any type.
  
Sample:

```c
owner int socket = 
    socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
```

Pointers have the additional advantage of having a natural zero semantics.   

Considering that the objective of owner objects is to control the lifetime of the referenced object, it does not make sense a owner pointer to functions.


> The exactly name of the new qualifiers still not defined. It can be something like `_Owner` or just `owner`.  My suggestion is to have macros for each qualifier to to be able to compile the same source code in compilers that don't support ownership checks. 

### Semantics

1 - When a owner object is copied to another owner object the ownership is always moved/transfered.

The object that receives the value must not hold any resource, otherwise the previous referenced objects would be lost.
This is natural in initialization and function arguments. 

Sample returning values

```c  
T* owner make_owner() {  
   T* owner  p = ...
   return p;
}
```
  
Sample initialization

```c
T * owner b = make_owner(); /*moved*/  
```

Sample calling functions

```c
void f(T * owner a);
f(a); /*moved*/
```

When passing const owner to non const the compiler can ignore the warning. This solves an old and annoying warning.


```c
const void * p = malloc(10);
free(p);
// warning: passing argument 1 of 'free' discards 'const' qualifier from pointer target type
```
  
The reason is because now the compiler has an extra information that is the owner qualifier.  

```c
void free(void * owner p);
```  

moving a const object to a function already means the object cannot be used anymore. The state is uninitiated, so as a consequence the warning is not necessary and can be removed.

Sample assignment

```c
T * owner b = make_owner();
T * owner a = 0;  
...
a = b; /*moved*/;  
```

Flow analysis must ensure a is not holding any resources.

2 - A __non owner object__ cannot be copied to a __owner object__.   
  
The only exception is the null pointer constant when copied to owner pointers.

```c  
T * owner f() {
  return 0; //OK
}
```

 3 - An __owner object__ can be copied to a __non-owner object__ if its lifetime is longer than that of the __non-owner object__.  
   
We call the non-owner object a "view" in this case. 
  
```c
T F()
{
    owner T local;
    return local; /*error*/
} 
```

```c  
owner T global;
T F() {
   return global; //OK
}
```

```c  
T F(owner T arg) {
   return arg; //OK
}
```

```c  
  void F(T * p);
  F(make_owner()); //ERROR  
```
  
```c 
  T  *p = make_owner(); //ERROR
``` 
  
```c 
  T  *p = 0;  
  {
    T * owner p2 = make_owner();  
    p = p2; //error  
    free(p2);
  }
```

4 - Temporary owner objects cannot be discarded/ignored  

```c  
  make_owner(); //ERROR  
```

```c  
  void f(T * owner ) /*ERROR no name*/  
  {
  }  
```


## Part II - Flow analysis

The objective of flow analysis is to ensure that when the lifetime of owner object ends, it must have  already released the resources it owns. The same when we overwrite the owner object.  

Sample:  

```c  
  FILE * owner f = fopen("file", "r");  
  if (f){  
      fclose(f);
  }
```
  
Flow analysis must know that when f goes of of scope it does not owns any resource. 

To track ownership fours states are used "uninitialized", "moved", "null" and "not null".  We can assert compile time states in cake using **static_state**
    
```c
T * p;  
static_state(p, "uninitialized");
```

We can also print object states using **static\_debug**.

```c
T * p;  
static_debug(p);
```

Pointers can have the state "null" or "not-null".  The combination of "null or not-null" is also called "maybe-null".  
 
```c
T * owner p = 0;  
static_state(p, "null");

T * owner p2 = malloc(sizeof(T));  
static_state(p2, "maybe-null");
```

When one owner object is copied to another the state of the source object is "moved".  The state of the destination object is the same of the previous state of the source object.

```c
T * owner p1 = 0;  
T * owner p2 = malloc(sizeof(T));  

static_state(p1, "null");
static_state(p2, "maybe-null");

p1 = p2;  

static_state(p1, "maybe-null");
static_state(p2, "moved");

```

The exception is when we move owner objects into functions, then the state is "uninitialized". This prevents use after free or concurrency problems.

```c  
  T * owner p = malloc(sizeof(T));  
  free(p);
  static_state(p2, "uninitialized");
```


The object state is a set of all possible states.  
  
Sample:  

```c  
  FILE * owner f = fopen("file", "r");  
  if (f) {  
      fclose(f);
  }  
  static_state(f, "null moved");
```

When an owning pointer goes out of scope and before its memory is overridden it cannot have "not-null" in its set state.   

All others possibilities are permitted. For instance, the state can be "null or uninitialized" or "null or moved".
  
When an owning object (non pointer) goes out of scope and before its memory is overridden it must be on "uninitialized" or "moved" or "uninitialized or moved" state.
Structs an unions doens't have a direct state. But we can say that some struct has been moved if all of its objects have been moved.


### Special case malloc/calloc 

```c
struct X {    
    char * owner name;
};

int main() {
  struct X * owner p = malloc(sizeof (struct X));
  p->name =  strdup("hi");
}
```

We must track that object `p->name` does not have any resources before the strdup assignment. We know malloc returns an uninitialized memory but the compiler does not known. 

For this particular case the semantics of malloc/calloc will be built-in; So the static analyzer will know malloc returns a uninitialized memory and calloc return a zeroed memory allowing us to write the code above.

### Disabling ownership checks  
  
Some functions can have a intricate logic of pointers and instead of trying to adapt the logic we know is correct, we can just annotate "unchecked" at function definition.  

```c
void f() "unchecked" {  
}
```
  
So far, from the experience of using this feature in cake source itself, I have much less usages of "unchecked" than I was expecting initially.

**assert** also can be used to override the static analysis state.

```c
/* linked list sample */

struct node {
 struct node* owner next;
};

struct list {
  struct node * owner head;
  struct node * tail;
};

void node_delete(struct node* owner p) {
  if (p) {
     assert(p->next == NULL);
     free(p);
  }
}

void list_destroy(struct list* obj_owner list)
{
  struct node * owner p = list->head;
  while (p) {
      struct node *  owner next = p->next;
      p->next = NULL;
      node_delete(p);
      p = next;
  }
}
```

list_destroy can be implemented as follow:

```c
void list_destroy(struct list* obj_owner list)
{
  struct node * owner p = list->head;
  while (p) {
      struct node *  owner next = p->next;
      free(p);
      p = next;
  }
}
```
  
We don't need assert here because compiler knows "p->next" has been moved. We don't have a way do override the static analysis state "moved" or "uninitialized" using assert.

We also have **static_set** that can be used where assert does not work. For instance:

```c
  void* owner pnew = realloc(p->data, n * sizeof(p->data[0]));
  if (pnew == NULL) return ENOMEM;
  static_set(p->data, "moved");
  p->data = pnew;
```



## Grammar

```c
 
 type-qualifier:
   ...
   'owner'
   'view'
   'obj_owner'

static_debug-declaration:
  'static_debug' (constant-expression) ;

static_state-declaration:
  'static_state' (constant-expression, literal ) ;

static-declaration:  /*where static-assert is used today*/
 static_assert-declaration
 static_debug-declaration
 static_state-declaration

TODO "unchecked"
```

## Checking your source code
I suggest the creation of a header file "ownership.h"

```c

#pragma once

#ifdef __CAKE__
#define owner owner
#define obj_owner obj_owner
#define view view
  
void* owner calloc(int nmemb, int size);
void free(implicit void* owner ptr);
void* owner malloc(int size);
void* owner realloc(void* owner ptr, int size);
char * owner strdup( const char *src );

#else
#define owner
#define obj_owner
#define view

#endif

```

Then include this header on the top of your files and just run cake.

The way I am thinking is..

I have variables, for instance  

int a = 1;  
int b = 2;
int c = 3;  

if (condition) {  
  a = 4;    
  b = 5;  
}
else {    
  a = 6;  
  c = 7;  
}    
  
// a is 4 or 6  
// b is 2 or 5  
// c is 3 or 7  


## Terms

### storage duration 
An object has a storage duration that determines its lifetime. There are four storage durations: static, thread, automatic, and allocated. 

### lifetime 

The lifetime of an object is the portion of program execution during which storage is guaranteed to be reserved for it. An object exists, has a constant address and retains its last-stored value throughout its lifetime.

If an object is referred to outside of its lifetime, the behavior is undefined.

If a pointer value is used in an evaluation after the object the pointer points to (or just past) reaches
the end of its lifetime, the behavior is undefined.

The representation of a pointer object becomes indeterminate when the object the pointer points to (or just past) reaches the end of its lifetime.

### value
precise meaning of the contents of an object when interpreted as having a specific type

### object
Region of data storage in the execution environment, the contents of which can represent values.

### owner object
Is an object which value represents a reference to other object which lifetime is controlled by it.

