

** Under development.**

# Ownership checks for C

The static ownership check is an experimental feature for Cake.
It consists of two separate implementations. 

The first implementation focuses on introducing the concept of an owner qualifier in the type system. 

The second implementation revolves around flow analysis, ensuring that owned resources are appropriately released when necessary.   
  
By implementing the static ownership check, and using the feature on it's own source, Cake aims to explore and evaluate the effectiveness of this feature.
  
## New Type qualifiers

```c
type-qualifier:
  ...
  _Owner
  _View
  _Obj_owner   
```

The **_Owner** qualifier can be used when declaring a variable to indicate that its value represents a reference to a resource that must be released exclusively through that reference.

The **_View** qualifier is the default for any variable, indicating that the variable is not responsible for releasing a resource, even if it has access to it. View qualified object does not control the lifetime of the resource, which must exist beyond the lifespan of the view qualified object itself.

The **_Owner** qualifier, when used with a pointer, indicates that the pointer assumes ownership of both the pointed object and its associated memory.

When converting a owner pointer to void*, only the ownership of the memory is moved.

```c
void * _Owner f1(){
  struct X * _Owner p = malloc(sizeof (struct X));
  return p; //error 
}
```

Conversely, the **_Obj\_owner** qualifier is exclusively applicable to pointers, signifying that the pointer owns the pointed object but not the memory it occupies.

For struct and unions, if at least one member has the **_Owner** qualifier, the entire type is considered to be an owner qualified type.

Sample:

```c
struct person {
  char * _Owner name;
};

int main(){
  struct person p1 = {};
  // same as _Owner struct person p1;
}
```

Applying the **_View** qualifier to an struct or union type designates the type as a view, regardless of whether it includes members owner qualified.

```c
struct person {
  char * _Owner name;
};

int main(){
  _View struct person p2;
   //p2      is _View qualified  
   //p2.name is _View qualified
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
void destroy_array(int n, struct person a[_Owner 10]);
```


## Initialization
### Owner = Owner

In this situation we always move the ownership.

When assigning from a owner variable we need explicitly add the **_Move** keyword. 

```c
_Owner T a = _Move b;        //OK
```
We have the ownership moved from b to a.

When assigning from a function result we don't use **_Move**.

```c
_Owner T make_owner();
_Owner T a = make_owner();
```
  

### Owner = Non-owner

We can initialize owner qualified pointer with the null pointer constant.

```c
T _Owner * p1 = 0;       //OK
T _Owner * p2 = NULL;    //OK
T _Owner * p3 = nullptr; //OK
```
  

### Non-Owner = Owner  

In this situation we always have a view.

```c  
_Owner T b;
T a = b;            //OK
```

We say "a is a view to b".

We cannot have a view for a object returned by a function.

```c
T a = make_owner(); //ERROR
```
  
## Assignment

### Owner = Owner

In this situation we always move the ownership. 

When assigning from a function result we need to use  **_Move**.

```c
a = _Move make_owner();   //OK
```


### Owner = Non-Owner
We can only assign null pointer constant to owner pointers.

```c
 T * _Owner p;
 p = 0;
```

### Non-Owner = Owner

Same of initialization, we have a view.

```c  
_Owner T b;
T a;
a = b;            //OK
```


## Return
  
### Owner F() return Owner  

When returning local storage variables we don't have to use **_Move**.

```c
_Owner T F() {
   _Owner T local;
   return local; //OK   
}
```

When returning non local storage variables (including function parameters) we need to use **_Move.**

```c  
_Owner T global;
_Owner T F() {
   return global;       //ERROR
   return _Move global; //OK   
}
```

```c  
_Owner T F(_Owner T arg) {
   return arg;       //ERROR
   return _Move arg; //OK   
}
```

When returning a owner type, the called cannot discard the result.

```c  
 F(); //ERROR discarding a owner result   
```


### Owner F() return Non-Owner

The only possible value is null pointer constant.

```c  
T * _Owner F() {
  return 0;        // OK, or nullptr or (void*)0)
}
```


### Non-Owner F() return Owner

We return a view of the object.

Returning a owner local storage variable is an error.

```c
T F()
{
    _Owner T local;
    return local; //ERROR    
} 
```

We can return non local variables. 

```c  
_Owner T global;
T F() {
   return global;       //OK (view)
}
```

```c  
T F(_Owner T arg) {
   return arg;         //OK (view)
}
```


## Calling functions
In general the rules are similar of initializing the parameter with the argument. 

### void F(Owner); F(Owner);

We explicitly use **_Move** on the caller.

```c
  void F(_Owner T a);
  
  _Owner T a;
  
  F(_Move a);      //OK
  F(make_owner()); //OK
  
```


We can use _Implicit  to make the usage of **_Move** optional. This is useful when the semantics of the function is very clear, for instance if the name of the function is "destroy".

```c
void x_destroy(_Implicit struct X * _Obj_owner);
```

(This can be changed to a attribute in the next version)


### void F(Owner); F(Non-Owner);
We can just use a null pointer constant.

```c
  F(0);            //nullptr, NULL, etc.  
```

### void F(Non-Owner); F(Owner);

Here, the difference from initialization is that we cannot pass owner object return by a function.

```c
  F(make_owner()); //ERROR  
```

```c
  _Owner T a; 
  F(a); //OK (view)
```


## Samples

Pointers

```c
struct person {
  char * _Owner name;
};  

void person_delete(_Implicit struct person * _Owner p) 
{
  if (p) {
    free(p->name);
    free(p);
  }
}

int main()
{
   struct person * _Owner p = malloc(sizeof * p);   
   person_delete(p);
}
```

Swap

```c
struct person {
  char * _Owner name;
};  

void person_swap(_View struct person * a,  
                 _View struct person * b) 
{
   _View struct person temp = *a;
   *a = *b;
   *b = temp;
}

void person_destroy(struct person * _Obj_owner p) 
{
  free(p->name);
}

int main()
{
   struct person p1 = {};
   struct person p2 = {};
   
   person_swap(&p1, &p2);
   person_destroy(&p1);
   person_destroy(&p2);
}
```

Linked list:

```c
void * _Owner calloc(int n, int sz);
void free(_Implicit void * _Owner);
#define NULL ((void*) 0)

struct book {
     char* _Owner title;
     struct book* _Owner next;
};

void book_destroy(_Implicit struct book* _Obj_owner book) {
     free(book->title);
}
 

struct books {
    struct book* _Owner head, *tail;
};


void books_push_back(struct books* books, struct book* _Owner new_book)
{
   if (books->tail == NULL) {
      books->head = _Move new_book;
   }
   else {
      books->tail->next = _Move new_book;
   }
   books->tail = new_book;
}

void books_destroy(_Implicit struct books* _Obj_owner books)
{
    struct book* _Owner it = books->head;
    while (it != NULL) {
        struct book* _Owner next = _Move it->next;
        book_destroy(it);
        free(it);
        it = _Move next;
    }
}

int main(int argc, char* argv[])
{
    struct books list = { 0 };
    struct book* _Owner b1 = calloc(1, sizeof(struct book));
    if (b1)
    {
        books_push_back(&list, _Move b1);
    }
    books_destroy(&list);
}

```


## Flow analysis

When owner objects goes out of scope, the flow analysis
must check if the object has been moved. 

```
{
  _Owner int a;
} //warning variable a not moved/destroyed
```

At the final destination, the object will not be moved anymore, then we annotate the function definition as [[no\_ownership\_checks]].

```c
[[no_ownership_checks]] free(void * _Owner p)
{
  //implementation of free....
}
```
  
### Sample 1

Simple sample with malloc and free.  

```c
void * _Owner malloc(int i);
void free(_Implicit void * _Owner p);

int main() {
   void * _Owner p = malloc(1);
   //free(p);
} //compiler will complain c not moved
```  

### Sample 2  

This sample shows move and returns at f and initialization at main.


```c
void * _Owner malloc(int i);
void free(_Implicit void * _Owner p);

struct X {
  int i;
};

 struct X * _Owner f() {
    struct X * _Owner p = malloc(1);
    struct X * _Owner p2 = _Move p;
    return p2; /*p2 is moved*/
}

int main() {
   struct X * _Owner p = f();
   //free(p);     
}
```


### Sample 3

This sample shows that we need to move the struct x at main and also shows that we must implement x_destroy correctly.

```c
char * _Owner strdup(const char *s);
void free(_Implicit void * _Owner p);

struct X {
  char *_Owner name;
};

void x_destroy(_Implicit struct X * _Obj_owner p) {
  //free(p->name);
}

int main() {
   struct X x = {0};
   x.name = _Move strdup("a");
   //x_destroy(&x);
}
```

### Sample 4

This sample shows that adding a new member name at struct requires we also free at main.

```c
void free(_Implicit void* _Owner ptr);
void* _Owner malloc(int size);

struct X
{
    int i;
    //char * _Owner name;
};

int main() 
{
    struct X * _Owner p = malloc(sizeof (struct X));
    free(p);
}

```
  
### Sample 5

This sample show the implementation of delete.

```c
void * _Owner malloc(int i);
void free(_Implicit void * _Owner p);

struct X {
  char * _Owner text;
};

void x_delete(_Implicit struct X * _Owner p) {
    free(p->text);
    free(p);    
}

int main() {
   struct X * _Owner p = malloc(sizeof(struct X));
   p->text = _Move malloc(10);
   x_delete(p);
}
```

### Sample 6
This sample show some details when we cast a owner pointer to void*.

```c
void free(_Implicit void* _Owner ptr);
void* _Owner malloc(int size);

struct X
{    
    char * _Owner name;
};

/*
  To remove this error return 
    struct X * _Owner 
  instead   of 
    void * _Owner.
*/
void * _Owner f1(){
  struct X * _Owner p = malloc(sizeof (struct X));
  return p;
}

```

## Grammar

```c
New keywords:
  _Move   
  _Owner   
  _View   
  _Obj_owner   
  _Implicit /*may become attribute*/

 type-qualifier:
   ...
   _Owner
   _View
   _Obj_owner   

parameter-declaration:
  attribute-specifier-sequence opt _Implicit opt                declaration-specifiers declarator  

  attribute-specifier-sequence opt _Implicit opt declaration-specifiers abstract-declarator opt

 argument-expression-list:
   move_opt assignment-expression
   argument-expression-list , assignment-expression

 init-declarator:
   declarator
   declarator = move_opt initializer

 assignment-operator:
  = 
  = move
  ...

 jump-statement:
  ...
  return;
  return move_opt expression;  

```

## Checking your source code
I suggest the creation of a header file "ownership.h"

```c

#pragma once

#ifdef __CAKE__
#define implicit _Implicit
#define owner _Owner
#define obj_owner _Obj_owner
#define move _Move
#define view _View
  
void* _Owner calloc(int nmemb, int size);
void free(_Implicit void* _Owner ptr);
void* _Owner malloc(int size);
void* _Owner realloc(void* _Owner ptr, int size);
char * _Owner strdup( const char *src );
char * _Owner strdup( const char *str1 );

#else
#define implicit
#define owner
#define obj_owner
#define move
#define view

#endif

```

Then include this header on the top of your files and just run cake.



