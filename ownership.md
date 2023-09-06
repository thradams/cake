# Static Ownership Checks in Cake
Cake is a C23 Front End http://thradams.com/cake/index.html that is implementing static ownership checks. 

Cake source itself is already using this feature that can be used and disabled with few macros. 
  
This is a tour explaining the usage of this feature.  

Cake compiler can be used online   
http://thradams.com/cake/playground.html
  
** This feature still under design and development.**

## Owner Objects

An **owner object** is an object responsible for holding a reference to another object and managing its lifetime.

The most common type of owner objects is pointers, often referred to as **owner pointers**. An owner pointer is created with the qualifier owner, as illustrated in Listing 1:

```c
FILE *owner f = fopen("file.txt", "r");
if (f)
    fclose(f);
```
Listing 1 - Owner Pointer to FILE

Each object has only one owner. When an owner object is copied, ownership is transferred. For example, in Listing 2, the ownership of the owner pointer `f` is transferred to `f2`:

```c
FILE *owner f = fopen("file.txt", "r");
FILE *owner f2 = f; /*MOVED*/
if (f2)
    fclose(f2);
```
Listing 2 - Assignment of Owner Objects is a Move

**Rule:** Before the end of its lifetime, owner objects must transfer the ownership of the objects they possess.

Invoking a function is analogous to assignment, resulting in the object’s transfer of ownership. In the previous examples, we were already transferring ownership of the owner pointer to the close function, which is declared as follows:

```c
void close(FILE *owner p);
```
Listing 3 - Declaration of close

### Other Types of Owner Objects

We can have other types of **owner objects**. For instance, Berkeley sockets use an integer to identify the socket, as shown in listing 4:

```c
owner int server_socket =
     socket(AF_INET, SOCK_STREAM, 0);
 /*...*/
 close(server_socket);
```
Listing 4 - Non-Pointer owners objects

## View Objects

A **view object** is an object responsible for holding references to other objects without taking ownership of those referenced objects. The most common view objects are pointers called **view pointers**. The view qualifier is not necessary to specify since it's the default behavior. When an owner object is copied to a view object, the ownership is not transferred, as shown in Listing 5:

```c
void use_file(FILE *f) {
}
int main() {
    FILE *owner f = fopen("file.txt", "r");
    if (f) {
        use_file(f); /*NOT MOVED*/
        fclose(f);
    }
}
```
Listing 5 - Calling Function with View Parameters

When a **view** qualifier is used in structs, it makes all members as view objects. Listing 6.

```c
struct X {   
  char *owner text;   
};  

void f(view struct X x) {  
    
}  

int main() {
    struct X x = {};
    f(x); /*NOT MOVED*/    
    free(x.text);
}
```
Listing 6 - A view parameter

## Deleting Owner Pointers

**Owner pointers** take on the responsibility of owning the pointed object and its associated memory, treating them as distinct entities. A common practice is to implement a delete function, as illustrated in Listing 7:

```c
struct X { 
  char *owner text; 
};
void x_delete(struct X *owner p) 
{
  if (p) {
    free( p-> text);
    free( p); 
  } 
 }
 int main() 
 {
   struct X * pX = calloc( 1, sizeof * pX);
   if (pX) {
     /*...*/;
     x_delete( pX); 
   }
}
```
Listing 7 - Implementing the delete function

In situations where we want to create the object on the stack, we can implement a destructor, as shown in Listing 8:


```c
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
Listing 8 - Implementing a destructor


In C, structs are typically passed by pointer rather than by value. To transfer the ownership of an object to a pointer, we introduce a new qualifier, **obj_owner**. 

A pointer qualified with **obj_owner** is the owner of the pointed object but not responsible for managing its memory. Listing 9 illustrates how to implement a destructor using a obj_owner pointer parameter.


```c
struct X {
    char *owner text;
};
void x_destroy(struct X *obj_owner x) {
    free(x->text);
}
int main() {
    struct X x = {};
    /*more code*/
    x_destroy(&x); /*x is moved*/
}
```
Listing 9 - Implementing a destructor using obj_owner

## Copying a owner pointer to a obj_owner pointer
 
We can copy an owner pointer to an **obj_owner** pointer. In this scenario, only the ownership of the pointed object is transferred, not the memory ownership. Listing 10 illustrates how we can use `x_destroy` in the implementation of `x_delete`.


```c
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
   struct X * pX = calloc( 1, sizeof * pX);
   if (pX) {
     /*...*/;   
     x_delete( pX); 
   }
 } 
```
Listing 10 - Using `x_destroy` to implement `x_delete`

## Static analysis  

All the rules we have presented are checked by the compiler. Let's revisit our first example:

```c
FILE *owner f = fopen("file.txt", "r");
if (f)
    fclose(f);
```

he flow analysis must ensure that when the owner pointer ‘f’ goes out of scope, it does not own any objects. At the end of the scope, ‘f’ can be either null or moved, and both states ensure that there are no remaining resources.

To check the ownership rules, the compiler uses five states:

- uninitialized
- moved
- null
- not-null
- zero
 
We can print these states using the **static_debug** declaration. We can also assert the variable is at a certain state using the **static_state** declaration. Listing 11 shows this usage:
  
 ```c
int main() {
 int a;   
 static_state(a, "uninitialized");  
 static_debug(a);
}
 ```  
Listing 11 - Usage of **static_state** and **static_debug**
  
Output:

```
source:5:2: note: static_debug
 5 | static_debug(a);
   | ^~~~~~~~~~~~
   a == {(uninitialized)}   
```
  
As we have just seen, the **uninitialized** state applies to variables that are declared but not initialized. The compiler ensures that we don't read uninitialized variables.

The **null** state means that owner objects are not referencing any object. Listing 12 shows a sample using owner pointers:

 ```c
int main() {
 void * owner p = nullptr;   
 static_state(p, "null"); 
}
 ```  
Listing 12 - Null state

The **not-null** state indicates that the object is referencing some object, as shown in listing 13.

```c
int main()
{
   void * owner p = malloc(1);
   if (p) {
     static_state(p, "not-null");
   }
}
```
Listing 13 - Not-null state

The **zero** state is used for non-owner objects to complement and support uninitialized checks.

```c
int main()
{
   int i = 0;
   static_state(i, "zero");   
}
```
Listing 14 - The zero state

**Zero** and **null** are different states. This difference is necessary because, for non-pointers like the socket sample, 0 does not necessarily mean null; instead, -1 represents the null state. The compiler does not know the semantics for types that are not pointers. However, you can use **static_set** to inform states. In Listing 15, we annotate that server_socket is null, which doesn't mean it is zero but indicates that it is not holding any resources and is safe to return without calling close.


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
Listing 15 - Usage of static_set

Now let's consider `realloc` function.

```c
void * owner realloc( void *ptr, size_t new_size );	
```

In the declaration of `realloc`, we are not moving the ptr. The reason for that is because the `ptr` may or may not be moved. If the function returns NULL, `ptr` was not moved. Listing 16 shows how **static_set** can be used.


```c
  void * owner p = malloc(1);
  void * owner p2 = realloc(p, 2);
  if (p2 != 0)
  {
     /*
        if p2 != 0 it  means p was moved.
     */
     static_set(p, "moved");
     p = p2;
  }    
  free(p);
```
Listing 16 - Using static_set with realloc


The state of an object is a combination of all possible states. For instance, let's print and check the state of `f` at listing 17.

```c  
int main() {
  FILE *owner f = fopen("file.txt", "r");
  if (f)
    fclose(f);  
  static_state(f, "null or moved");
  static_debug(f);
}
```
Output:

```c
source:13:3: note: static_debug
 13 |  static_debug(f);
    |  ^~~~~~~~~~~~
    f == {*(null moved)}
```
Listing 17 - Flow analysis

All rules are a consequence of your objective.   
  
**Rule:** We cannot discard owner objects as showed in listing 18.  

```c  
void using_file(FILE * f) {
}
int main() {  
 fopen("file.txt", "r"); //ERROR  
 using_file(fopen("file.txt", "r")); //ERROR
}
```
Listing 18 - owner objects cannot be discarded.

**Rule:** Before the assignment of owner objects, the compiler checks if the owner object is not holding any resource, as shown in Listing 19:

```c
  FILE * owner file = fopen("file.txt", "r");  
  file = fopen("file.txt", "r"); //ERROR
```
Listing 19 - Check before assignment

Sometimes is not possible to infer the state. Consider this sample.
  
```c
struct X {
  char *owner text; 
};

void init(struct  X * x) {
  x->text = strdup("a");
}
void set_text(struct  X * x) {
  x->text = strdup("a");
}
```
Listing 20 - Some states cannot be inferred

How to know `x->text` was not holding any resource? At  listing 21, we show how **static_set** can be used to say `x->text` is not initialized at `init`, and inside `set_text` how compile will recognize the runtime assert to ignore the error that `x->text` may be not null. 

```c
void init(struct  X * x) {  
  static_set(x->text, "uninitialized");
  x->text = strdup("a");
}  
void set_text(struct  X * x) {  
  assert(x->text == NULL); /*NOT RECOMENDED HERE*/
  x->text = strdup("a");
}
```
Listing 21.
 
At set_text the best option is just release the previous resources as showed at listing 22.

```c
void set_text(struct  X * x) {  
  free(x->text);
  x->text = strdup("a"); //OK!
}
```

Obs: Its being considered a attribute [[out]], this attribute can tell the compiler the parameter can be uninitialized. Listing 22 shows a possible declaration. 

```c
void init([[out]] struct  X * x);
```

Continuing with the rules.
  
**Rule:** A non-owner object cannot be copied to a owner object as showed in listing 23

```c
FILE * f();
int main() {  
   FILE * owner file = f(); //ERROR   
   FILE * owner file2 = 0;  //OK
}
```
Listing 23 - non owner cannot be copied to owner
  
The exception is the null pointer constant.

**Rule:** A view parameter cannot leave the scope with moved/uninitialized objects. Listing 24

```c
void f(struct Y * p) {
    x_destroy(p->px);
} //ERROR p->x is leaving scope MOVED
  
int main() {
   struct Y  y = {};
   y.x.name = strdup("a");
   f(&y);
   free(y.x.name);
}
```
Listing 24 - Messing with view parameters

However, listing 25 is correct.
  
```c
void x_swap(struct X * a, struct X * b) 
{
  struct X temp = *a;
  *a = *b;
  *b = temp;
}   
/*  
 OK because a and b are not leaving scope   
 with moved objects  
*/
```
Listing 25 - swap function

**Rule:** When objects are moved to functions, they become uninitialized. This prevents bugs like double free. Listing 26. 

```
 int main() {
  void * owner p = malloc(1);
  free(p);
  free(p); //ERROR p is uninitialized
}
```


**Rule:** moved objects cannot be moved. Listing 27.

```c
int * owner p1 = ...;
int * owner p2 = p1;
int * owner p3 = p1; //ERROR p1 was moved
```
Listing 27 - We cannot move a moved object

**Rule:** When coping a owner object to to a view object the compiler must check if the lifetime. Listing 28

```c
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
}
```
Listing 28 - Lifetime check

**Rule:** const can be discard when moving to `void*`. Listing 29

```c
void free(void * owner p);
int main(){
  const void * p = malloc(1);
  free(p); //no discarding const warning here
}
```
Listing 29 - Non warning discarding const.

**Rule:** To convert a view pointer to obj_owner we need check that the origin (the object it is pointing) is an owner object. Listing 30.

```c
struct X {
 struct Y * p;
};
void y_destroy(struct Y * obj_owner p);
void f(struct X * x) {
  y_destroy(x->y); //ERROR origin of y cannot be verified
}
```
Listing 30 -Checking the origin of obj_owner
  
**Rule** When we cast void pointer to objects we assume the object is uninitialized. Listing 31  

```c
struct X { 
  char *owner text; 
};
void x_delete(struct X *owner p);  

int main() 
{
   struct X * pX = calloc( 1, sizeof * pX);
   if (pX) {
     static_state(pX->text, "uninitialized");
     p->text = strdup("hi"); 
     x_delete( pX); 
   }
}
```
Listing 31 - void objects are uninitialized

When calloc is used, the memory is initialized with zeros.
if necessary, **state_set** can be used with "zero" parameter to tell the compiler the state of the pointed object. Listing 32.

```c
struct X { 
  char *owner text; 
};
int main() 
{
   struct X * pX = calloc( 1, sizeof * pX);
   if (pX) {  
     /*the memory of pointed object is all zeroes*/
     static_state(*p, "zero");          
   }
}
```
Listing 32 - Zeroing the state

**Rule**: An object that has any parts moved/uninitialized cannot be moved.  Listing 33.

```c
struct X { 
  char *owner part1;
  char *owner part2;  
};
void f(struct X x);

int main() 
{
  struct X x = {};
  /*..*/
  free(x.part1);
  f(x); //ERROR x.part1 is uninitialized
}
```
Listing 33 - Messing with owner objects 



