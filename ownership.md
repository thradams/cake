

** Under development.**

# Ownership checks for C

The static ownership check is an experimental feature for Cake.
It consists of two separate implementations. 

Part I focuses on introducing the concept of an owner qualifier in the type system and does not requires flow analysis. 

Part II requires flow analysis, ensuring that owned resources are appropriately released when necessary.   
  
By implementing the static ownership check, and using the feature on it's own source, Cake aims to explore and evaluate the effectiveness of this feature.  
  
## Part I - Type system changes  

### New qualifiers

```c
type-qualifier:
  ...
  owner
  view
  obj_owner
```

> The exactly name of the new qualifiers still not defined. It can be something like `_Owner` or just `owner`.   In any case, my suggestion is to have macros for each qualifier to to be able to compile the same source code in compilers that don't support ownership checks. 

The **owner** qualifier can be used when declaring a variable to indicate that its value represents a reference to a resource that must be released exclusively through that reference.

The **view** qualifier is the default for any variable, indicating that the variable is not responsible for releasing a resource, even if it has access to it.   

View qualified objects does not control the lifetime of the resource, which must exist beyond the lifespan of the view qualified object itself.

The **owner** qualifier, when used with a pointer, indicates that the pointer assumes ownership of the pointed object and its associated memory.   
  
For this reason, when converting a owner pointer to `void*`, only the ownership of the memory is moved.
  
For instance,  

```c
void * owner f1(){
  struct X * owner p = malloc(sizeof (struct X));
  p->name = strdup("a");
  return p; 
}
```
  
Returning a `void *` may leak `p->name` so we need a explicit cast `return (void * owner) p` if we want to do that.

The **obj\_owner** qualifier is exclusively applicable to pointers, meaning that the pointer owns the pointed object but not the memory it occupies. It is generally used in destructors.    
  
> It was not defined yet if this qualifier makes sense (is useful) in other scenarios other than function parameters.
  
```c    
void x_destroy(struct X * obj_owner p) {  
  //p is the onwer of struct X but it is not 
  //the owner of the memory X occupies.
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
   //p2      is view qualified  
   //p2.name is view qualified
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

The most common usage of **owner** will be pointers. However, ownership qualifiers can be applied for any type. One reason is because we can use other types as references to resources, for instance, we can use a integer.

```c  
owner int handle = make_resource();
release_resource(handle);
```
  
Pointers have the additional advantage of having a natural zero semantics.   
  
What is the empty value for handle for instance?

Obs:
owner pointer to function should be error.

The owner and obj_owner qualifiers should not be applicable to function objects.  

### Semantics

1 - When a owner object is copied to another owner object the ownership is always moved/transfered.

Copy happens when we return values, call functions, initialization and assignment.

Samples:

```c  

T* owner make_owner() {  
   T* owner  p = ...
   return p; /*moved*/
}

T * owner b = make_owner(); /*moved*/
T * owner a = 0;
a = b; /*moved*/;  

void f(T * owner a);
f(a); /*moved*/
```

The object that receives the value must not hold any resource. This is natural in initialization and function arguments. 

For assignment it needs to be checked with flow analysis (part II).


2 - A non owner object cannot be copied to a owner object.   
  
The only exception is the null pointer constant when copied to owner pointers.

```c  
T * owner f() {
  return 0; //OK
}
```

 3 - An owner object can be copied to a non-owner object if its lifetime is longer than that of the non-owner object.  
   
   We can the non-owner object a "view" in this case. 
  
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

The objective of flow analysis is to ensure that when an owning object goes out of scope or before its memory is overridden, it has released all of its resources.

    
Sample:  

```c  
  FILE * owner f = fopen("file", "r");  
  if (f){  
      fclose(f);
  }
```
  
Flow analysis must know that when f goes of of scope it does not owns any resource. 

>Note: In cake, we can print the object state   as seen by flow analysis using **static\_debug(expression)**.
We also can assert some specific state using **static\_state(expression, "state")**.
  
To track ownership fours states are used "uninitialized", "moved", "null" and "not null".  
    
  
```c
T * p;  
/* p is uninitialized*/  
```

Pointers can have the state "null" or "not-null".  The combination of "null or not-null" is also called "maybe-null".  
 
```c
T * owner p = 0;  
/* p is null*/  

T * owner p2 = malloc(sizeof(T));  
/* p2 is maybe-null*/
```

When one owner object is copied to another the state of the source object is "moved".  The state of the destination object is the same of the previous state of the source object.

```c
T * owner p = 0;  
T * owner p2 = malloc(sizeof(T));  
  
/*p1 is null*/  
/*p2 is maybe-null*/
  
p = p2;  

/*p2 is moved*/  
/*p1 is maybe-null*/

```

The exception is when we move owner objects into functions, then the state is "uninitialized". This prevents use after free or concurrency problems.

```c  
  T * owner p = malloc(sizeof(T));  
  free(p);
  /* p is uninitialized*/
```


The object state is a composition (set) of all possible states.  
  
Sample:  

```c  
  FILE * owner f = 0;  
  if (condition){    
      f = fopen("file", "r");
      fclose(f);
  } /*f is "null" or "moved" */
```

When an owning pointer goes out of scope and before its memory is overridden it cannot have "not-null" in its set state.   

All others possibilities are permitted. For instance, the state can be "null or uninitialized" or "null or moved".
  
When an owning object (non pointer) goes out of scope and before its memory is overridden it must be on "uninitialized" or "moved" or 
"uninitialized or moved" state otherwise we have a leak.


```c  
owner int handle = make_resource();
release_resource(handle);
``` 
  
We don't have semantics information to know for instance if handle 0 is valid or not.

Each member of aggregate objects is checked individually. 

### malloc/calloc 
A common initialization is.

```c
struct X {    
    char * owner name;
};

int main() {
  struct X * owner p = malloc(sizeof (struct X));
  p->name =  strdup("hi");
}
```

We must track that object `p->name` does not have any resources before the strdup assignment. We know malloc returns an uninitialized memory but the compiler does not known.  For this particular case the semantics of malloc/calloc will be built-in; So the static analyzer will know malloc returns a uninitialized memory and calloc return a zeroed memory.

### Disabling ownership checks  
  
Some functions can have a intricate logic of pointers and instead of trying to adapt the logic we know is correct, we can just annotate "unchecked" at function definition.  

```c
void f() "unchecked" {  
}
```
  
> This is also expected to be used with a macro, so compiler without ownership checks will just ignore.

assert can be used to override the static analysis state.

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
     /*  
       flow analysis will assume   
       p->next is null and safe to be   
       ignored  
     */
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

TODO

```c
void f(struct X *p){
  move(p->item);
} //error 'p->item' was left uninitialized  
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

  
