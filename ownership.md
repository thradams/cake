

** Under development.**

# Ownership checks for C

The static ownership check is an experimental feature for Cake.
It consists of two separate implementations. 

The first implementation focuses on introducing the concept of an owner qualifier in the type system. 

The second implementation revolves around flow analysis, ensuring that owned resources are appropriately released when necessary.   
  
By implementing the static ownership check, and using the feature on it's own source, Cake aims to explore and evaluate the effectiveness of this feature.

  
Once you experience checked coded, there's no turning back!

  
## Part I - Type system changes  

### New Type qualifiers

```c
type-qualifier:
  ...
  _Owner or owner
  _View or view
  _Bbj_owner  or obj_owner
```

The **owner** qualifier can be used when declaring a variable to indicate that its value represents a reference to a resource that must be released exclusively through that reference.

The **view** qualifier is the default for any variable, indicating that the variable is not responsible for releasing a resource, even if it has access to it. View qualified object does not control the lifetime of the resource, which must exist beyond the lifespan of the view qualified object itself.

The **owner** qualifier, when used with a pointer, indicates that the pointer assumes ownership of both the pointed object and its associated memory.

When converting a owner pointer to void*, only the ownership of the memory is moved.

```c
void * owner f1(){
  struct X * owner p = malloc(sizeof (struct X));
  return p; //error 
}
```

Conversely, the **_Obj\_owner** qualifier is exclusively applicable to pointers, signifying that the pointer owns the pointed object but not the memory it occupies.
> Design note: Maybe an alternative to **_Obj\_owner** could references for C, but only in function parameters.  
 
For struct and unions, if at least one member has the **owner** qualifier, the entire type is considered to be an owner qualified type.

Sample:

```c
struct person {
  char * owner name;
};

int main(){
  struct person p1 = {};
  // same as owner struct person p1;
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


### Initialization and assignment
#### Owner = Owner

Because we must have only one owner, in this situation the ownership is moved.  
  
Although, not necessary, a new keyword **move** was added to make this operation more explicit.

```c  
owner T b;

//Initialization
owner T a = move b;
```

```c  

owner T b;
owner T a;

//Assigment
a = move b;
```

The exception of the usage of **\move** is when initializing from a function result.

```c
owner T make_owner();
owner T a = make_owner();
```

In assignments from functions we need to use **move**.
  
```c  
a = move make_owner();  
```

This reminds us about the end of lifetime of a.

#### Owner = Non-owner
  
This is not allowed.  

The exception is the initialization/assignment of owner pointers to the null pointer constant. 

```c
T owner * p1 = 0;       //OK
T owner * p2 = NULL;    //OK
T owner * p3 = nullptr; //OK
```

```c
T owner * p1;
p1 = 0; //OK
```

> Design note: This could be extended for non pointers. The problem is we need a extra annotation to define what "empty" means for values other than non pointers. For instance, 0 can be valid index of a owner handle.   


#### Non-Owner = Owner  

In this situation we always have a view.

```c  
owner T b;
T a = b;            //OK
```

```c  
owner T b;
T a;
a = b;            //OK
```

We say "a is a view to b".

We cannot have a view for a owner objects with the storage duration shorter than the view.

```c
T a = make_owner(); //ERROR
```

```c
T a;
a = make_owner(); //ERROR
```
  
```c  
T global;
void f()  
{    
 owner T a;
  global = a; //ERROR
}
```

```c  

void f()  
{  
   T v;    
   {
      owner T a;
      v = a; //ERROR
   }
}
```  

(The last samples are not implemented yet in cake)

### Returning variables
  
#### Owner F() return Owner  

When returning local storage variables we don't use **move**.

```c
owner T F() {
   owner T local;
   return local; //OK   
}
```

When returning non local storage variables (including function parameters) we need to use **move.**

```c  
owner T global;
owner T F() {
   return global;       //ERROR
   return move global; //OK   
}
```

```c  
owner T F(owner T arg) {
   return arg;       //ERROR
   return move arg; //OK   
}
```

When returning a owner type, the caller cannot discard the result.

```c  
 F(); //ERROR discarding a owner result   
```


#### Owner F() return Non-Owner

The only possible value is null pointer constant.

```c  
T * owner F() {
  return 0;        // OK, or nullptr or (void*)0)
}
```


#### Non-Owner F() return Owner

We return a view of the object.

Following the general rule that view object duration must be shorter than the owner object, returning a owner local storage variable is an error.

```c
T F()
{
    owner T local;
    return local; //ERROR    
} 
```

However, we can return non local variables including parameters 

```c  
owner T global;
T F() {
   return global;       //OK (view)
}
```

```c  
T F(owner T arg) {
   return arg;         //OK (view)
}
```


### Calling functions
In general the rules are similar of initializing the parameter with the argument. 

#### void F(Owner); F(Owner);

We explicitly use **move** on the caller.

```c
  void F(owner T a);
  owner T a;
  F(move a);      //OK
```

We can use **\implicit**  to make the usage of **move** optional. This is useful when the semantics of the function is clear looking at it name, for instance, if the name of the function is "destroy" or "free" etc.    

Sample:
```c
void x_destroy(implicit struct X * obj_owner);
```

 > Attributes are begin considered for implicit

The exception of using **move** is when passing the function result directly.
  
```c
  void F(owner T a);
  F(make_owner()); //OK
```

  

#### void F(Owner); F(Non-Owner);
This is not allowed.  
The exception is the null pointer constant.

```c
  F(0); //OK  
```

#### void F(Non-Owner); F(Owner);

Following the general rule that view object duration must be shorter than the owner object, we cannot pass owner object return by a function.

```c
  F(make_owner()); //ERROR  
```
  
But we can pass variables, and the function parameter is a view of the argument.  

```c
  owner T a; 
  F(a); //OK (view)
```


## Part II - Flow analysis

The objective of flow analysis is to ensure that __when an owner object goes out of scope or when it is assigned it does not owns any resources__.
  
Not having resources means that the object is in one of these states:
  
 - uninitialized
 - moved
 - zero (for owner pointers)  
 - or any combination (zero or moved state)

The other possible states are:  

- unknown
- unknown but not zero
  
We can print (**static\_debug**) or check (**static\_state**)these states in compile time.

Sample:

```c
int main()
{
  int * i;  
  static_state(p, "uninitialized");   

  void * owner p = 0;  
  static_state(p, "zero");  
  static_debug(p);
}
```

A common initialization of heap allocated objects is using malloc/calloc.

```c
struct X {    
    char * owner name;
};

int main() {
  struct X * owner p = malloc(sizeof (struct X));
  p->name = move strdup("hi");
}
```
How to known that p->name was not holding resources? The semantics of malloc (returning a non initialized memory) must be understood. malloc/calloc will be especial cases where compiler will have the built in semantic. In  other situations is expect the programmer to silence the error.

## Grammar

```c
New keywords:
  'move'   
  'owner'   
  'view'   
  'obj_owner'   
  'implicit'

 type-qualifier:
   ...
   'owner'
   'view'
   'obj_owner'

parameter-declaration:
  attribute-specifier-sequence opt 'implicit' opt declaration-specifiers declarator  

  attribute-specifier-sequence opt 'implicit' opt declaration-specifiers abstract-declarator opt

 argument-expression-list:
   'move' opt assignment-expression
   argument-expression-list , assignment-expression

 init-declarator:
   declarator
   declarator = 'move' opt initializer

 assignment-operator:
  = 
  = move
  ...

 jump-statement:
  ...
  'return';
  'return' 'move' opt expression;  
 
static_debug-declaration:
  'static_debug' (constant-expression) ;

static_state-declaration:
  'static_state' (constant-expression, literal ) ;

static-declaration:  /*where static-assert is used today*/
 static_assert-declaration
 static_debug-declaration
 static_state-declaration

```

## Checking your source code
I suggest the creation of a header file "ownership.h"

```c

#pragma once

#ifdef __CAKE__
#define implicit implicit
#define owner owner
#define obj_owner obj_owner
#define move move
#define view view
  
void* owner calloc(int nmemb, int size);
void free(implicit void* owner ptr);
void* owner malloc(int size);
void* owner realloc(void* owner ptr, int size);
char * owner strdup( const char *src );
char * owner strdup( const char *str1 );

#else
#define implicit
#define owner
#define obj_owner
#define move
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

  
