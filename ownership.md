

** Under development.**

# Ownership checks for C

The static ownership check is an experimental feature in Cake.
It consists of two separate implementations. 

Part I focuses on introducing the concept of an owner qualifier in the type system and does not requires flow analysis. 

Part II requires flow analysis, ensuring that owned resources are appropriately released when necessary.   
  
By implementing the static ownership check, and using the feature on it's own source, Cake aims to explore and evaluate the effectiveness of this feature.  
  
## Part I - Type system changes  

### New qualifiers

  - owner
  - view
  - obj_owner

An **owner object** is an object that has at least one reference to another object, over which it has control of the lifetime. An owner object is the unique owner of the referenced object(s).
The **owner** qualifier is used to declare an **owner object**.

The most common owner objects are pointers, and we can also call them **owner pointers**.

```c  
  struct X * owner p = calloc(1, sizeof * p);    
  //...
  free(p);
```
    
We can have other types of **owner objects** that work as references but are not necessarily pointers. 
For instance, Berkeley sockets, uses a integer to identity the socket.
  
```c  
    owner int server_socket =   
       socket(AF_INET, SOCK_STREAM, 0);  
    ...   
    close(server_socket);
```
  
Aggregates (struct/unions/arrays) also can be a **owner object**.  

```c  
struct X {
  char * owner name;
  struct Y * owner p;
}; 
char * owner names[10];
```

**Owner pointers** assume ownership of the pointed object and its memory, treating them as if they were two different objects.

```c      
struct X { char * owner text; };   
int main() {
  struct X * owner p = malloc(1);  
    
  /*releasing object resources*/  
  free(p->text);
  
  /*releasing memory*/  
  free(p);  
}
```

When owner objects are copied the ownership is moved.  

For instance:

```c      
struct X { char * owner text; };   
int main() {
  struct X x1 = {};
  ....
  struct X x2 = {};
  x2 = x1;
  
  free(x2.text);
}
```
  
A **view object** is an object that holds references to other objects but does not own those references, which must exist beyond the lifespan of the view object itself. A **view pointer** is a pointer that does not own the pointed object, that is the default.    

The view qualifier is used to override the owner qualifier in structs.
  
```c      
struct X { char * owner text; };   
int main() {
  struct X x1 = {};
  ....
  view struct X x2 = {};
  x2 = x1; //x2 is just a view
  
  free(x1.text); //x1 still the owner
}
```

We can create destructors like this
  
```c    
struct X { char * owner text; };  
void x_destroy(struct X x) {
  free(p->text);
} //ok

int main() {
  struct X x= {0};
  //...
  x_destroy(x); //x moved
}
```

However, generally structs are passed by pointer in C. The next qualifier **obj_owner** is used for this purpose.
  
An **object owner pointer** is a version of **owner pointer** with the difference that is is responsible only for the lifetime of the pointed object, but not responsible for it's memory lifetime.   
  
The **obj\_owner** qualifier is used exclusively with pointers. The most common usage of **object owner pointer** is when creating destructors.  
  
Sample:

```c    
struct X { char * owner text; };  
void x_destroy(struct X * obj_owner p) {
  free(p->text);
}  

int main() {
  struct X x= {0};
  ...  
  x_destroy(&x); //x was moved
}
```

Now consider this sample that is not using **obj_owner**

```c    
struct X { char * owner text; };  
void f(struct X * p) {
  free(p->text);
}  
int main() {
  struct X x= {0};
  f(&x);
}
```
  
There is nothing on the function declaration telling us or the compiler that parts of x will be moved.  

So, what is checked is:  
**pointers to owner objects** cannot leave the scope with the pointed objected moved or parts of it moved.
  
This sample below is correct.  

```c    
struct X { char * owner text; };  
void f(struct X * p) {
  free(p->text);  
  p->text = strdup("new value");
} //ok p->text is leaving the scoped initialized   
```
  
But

```c    
void f(struct X * p) {
  free(p->text);
}  //error pointed object p->text was moved
```

is not correct.

**object owner pointer** to **non owner objects** does not make sense.   

**owner objects** must use complete types.
**owner pointers** to functions are not allowed.
   
### Semantics

1 - When a **owner object** is copied to another **owner object** the ownership is moved.
  
The destination owner object must not hold any resource at the moment of copy. This is natural on initialization, function arguments and returning objects. For assignment, flow analysis is required.    

Sample - returning values

```c  
T* owner make_owner() {  
   T* owner  p = ...
   return p;
}
```
  
Sample  - initialization

```c
T * owner b = make_owner();  
```

Sample  - calling functions

```c
void f(T * owner a);
f(a);
```

When passing a owner pointer that points to const object to a owner pointer to non-const object the common warning discards 'const' qualifier can be ignored.

```c  
void fred(void * owner p);
const void * owner p = malloc(10);
free(p); //no warning
```
  
Sample - assignment

```c
T * owner b = make_owner();
T * owner a = 0;  
...
a = b;  
```

Flow analysis must ensure `a` is not holding any resources before the assignment.
  
2 - When a **owner pointer** is copied to a **owner pointer** to `void` only the ownership of the object is moved.
  
The pointed object also must have been released before.  

```c      
struct X { char * owner text; };   
int main() {
  struct X * owner p = malloc(1);  
  free(p->text); //moving the ownership of object
  free(p); //moving ownership of the memory  
}
```

3 - When a **owner pointer** is copied to a **object owner pointer** the ownership of the pointed object is moved, but not the ownership of the memory.

```c    
struct X { char * owner text; };  
  
void x_destroy(struct X * obj_owner p){
  free(p->text);
}  

int main() {
  struct X x= {0};
  ...  
  x_destroy(&x); //object moved
  free(p); //memory moved
}
```

4 - We can pass a **pointer to owner object** as a **object owner pointer**. The object is moved.  

```c
 struct X{
   char * owner text;
 };
 void x_destroy(struct X * obj_owner p){
  free(p->text);
}  
 int main() {
   struct X x = {0};
   x_destroy(&x);//OK
 }
```
  
But arguments cannot leave the scoped moved.

5 - We cannot pass **pointer to non owner object** as a **object owner pointer**.

```c
 struct X{
   char * owner text;
 };
 void x_destroy(struct X * obj_owner p){
  free(p->text);
}  
 int main() {
   view struct X x = {0};
   x_destroy(&x);//ERROR
 }
```


6 - A **non owner object** cannot be copied to a **owner object**.   
  
The only exception is the null pointer constant when copied to owner pointers.

```c  
T * owner f() {
  return 0; //OK
}
```

```c  
int * p;
free(p); //error free requires an owner pointer
```

```c    
void x_delete(struct X * owner p);
struct X x;
x_delete(&x); //error
```

7 - An **owner object** can be copied to a **non-owner object** if its lifetime is longer than that of the **non-owner object**.  
  
  
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

8 - Temporary **owner objects** cannot be discarded  

```c  
  make_owner(); //ERROR  
```

```c  
  void f(T * owner )//ERROR
  {
  }  
```


## Part II - Flow analysis

The objective of flow analysis is to ensure that when the lifetime of **owner object** ends, it must have  already released the resources it owns.   
The same when we overwrite the **owner object**.  

Sample:  

```c  
  FILE * owner f = fopen("file", "r");  
  if (f){  
      fclose(f);
  }
```
  
Flow analysis must know that when f goes of of scope it does not owns any resource. 

To track ownership fours states are used

- uninitialized
- moved
- null
- not null    
  
We can assert compile time states in cake using **static_state**

Sample:

```c
T * p;  
static_state(p, "uninitialized");
```

We can also print object states using **static\_debug**.

```c
T * p;  
static_debug(p);
```

References can have the state "null" or "not-null".

The combination of "null or not-null" is also called "maybe-null".  
 
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
Structs an unions doesn't have a direct state. But we can say that some struct has been moved if all of its objects have been moved.


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


### Samples

```c
int mtx_init( owner mtx_t* mutex, int type );  
void mtx_destroy( mtx_t * obj_owner mutex );  

owner mtx_t mtx;
int r = mtx_init(&mtx, mtx_plain);  
if (r == thrd_success){  
  static_set(mtx, "not null");  
  mtx_destroy(&mtx);  
}
```
