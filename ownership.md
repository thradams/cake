

** Under development.**

# Ownership checks for C

The static ownership check is an experimental feature for Cake.
It consists of two separate implementations. 

The first part focuses on introducing the concept of an owner qualifier in the type system. 

The second part requires flow analysis, ensuring that owned resources are appropriately released when necessary.   
  
By implementing the static ownership check, and using the feature on it's own source, Cake aims to explore and evaluate the effectiveness of this feature.  
  
## Part I - Type system changes  

### New Type qualifiers

```c
type-qualifier:
  ...
  _Owner or owner
  _View or view
  _Bbj_owner  or obj_owner
```

> The exactly name of the new keywords still not defined. It can be something like `_Owner` or just `owner`. In any case my suggestion is to have macros that can help transiting from one compiler that  has ownership analysis to another without, just making empty macros. 

The **owner** qualifier can be used when declaring a variable to indicate that its value represents a reference to a resource that must be released exclusively through that reference.

The **view** qualifier is the default for any variable, indicating that the variable is not responsible for releasing a resource, even if it has access to it. View qualified objects does not control the lifetime of the resource, which must exist beyond the lifespan of the view qualified object itself.

The **owner** qualifier, when used with a pointer, indicates that the pointer assumes ownership of the pointed object and its associated memory. For this reason, when converting a owner pointer to `void*`, only the ownership of the memory is moved.
  
For instance,  

```c
void * owner f1(){
  struct X * owner p = malloc(sizeof (struct X));
  p->name = strdup("a");
  return p; 
}
```
  
Returning a `void *` may leak `p->name` so we need a explicit cast `return (void * owner) p` if we want to do that.

The **_Obj\_owner** qualifier is exclusively applicable to pointers, signifying that the pointer owns the pointed object but not the memory it occupies. It is generally used in destructors.  
  
```c    
void x_destroy(implicit struct X * obj_owner p) {  
  //p is the onwer of struct X but it is not 
  //the owner of the memory X occupies.
}  

int main() {  
  struct X x = {};
  ...  
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
  // same as 
  // owner struct person p1;
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

The most common usage of **owner** will be pointers. However the qualified can be applied for any type. One reason is because we can use other types as references to resources, for instance.

```c  
owner int handle = make_resource();
release_resource(handle);
```
  
But pointers have the additional advantage of having a natural zero semantics. What is the empty value for handle for instance?


### Initialization and assignment
#### Owner = Owner

Because we must have only one owner for each resourse, in this situation the ownership is always moved.  
  
Although, not necessary, a new keyword **move** was added to make this operation more explicit.

```c  
owner T a = move b;
```

```c  
owner T b;
owner T a;

a = move b;
```

The exception of the usage of **\move** is when initializing from a function result.

```c
T* owner  make_owner();
T * owner  a = make_owner();
```
  
Note: I have considered to make **owner** optional when initializing from function results because we cannot have a view object if the function returns an owner pointer.
```c
T * a = make_owner();
```

In assignments from functions we need to use **move**.
  
```c  
a = move make_owner();  
```



#### Owner = Non-owner
  
This is not allowed. The exception is the initialization/assignment of owner pointers to the null pointer constant. 

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

We cannot have a view for a owner objects with the storage duration shorter than the view object.

```c
T a = make_owner(); //error a cannot be a view
```

```c
T a;
a = make_owner(); //error a cannot be a view
```
  
```c  
T global;
void f()  
{    
  owner T a;
  global = a; //error global duration is bigger than a
}
```

```c  
void f()  
{  
   T v;    
   {
      owner T a;
      v = a; //error v duration is bigger than a
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

The objective of flow analysis is to ensure that __when an owner object goes out of scope or before assignment it does not owns any resources__.
 
We can print the object state as seen by flow analysis using **static\_debug**. We also can assert some specific state using **static\_state**.

Sample:

```c
int main()
{
  int * i;  
  static_state(p, "uninitialized");   

  void * owner p = 0;  
  static_state(p, "null");  
  static_debug(p);
}
```

### malloc/calloc 
A common initialization is.

```c
struct X {    
    char * owner name;
};

int main() {
  struct X * owner p = malloc(sizeof (struct X));
  p->name = move strdup("hi");
}
```

We must track that object `p->name` does not have any resources before the strdup assignment. We know malloc returns an uninitialized memory but the compiler does not known.  For this particular case the semantics of malloc/calloc will be built-in; So the static analyzer will know malloc returns a uninitialized memory and calloc return a zeroed memory.

### Disabling ownership checks  
  
Some functions can have a intricate logic of pointers and instead of trying to adapt the logic we know is correct, we can just annotate "unchecked" at function definition.  

```c
void f() "unchecked" {  
}
```
  
This is also expected to be used with a macro, so compiler without ownership checks will just ignore.

In some case, we need to disable just some parts of the code and not the entire function.

```c
void token_delete(implicit struct token* owner p)
{
    if (p) {
        [[already_released]] p->next;
        free(p->lexeme);
        free(p); //warning next not deleted
    }
}
```


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

TODO "unchecked"
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

  
