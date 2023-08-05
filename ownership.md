

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
> Design note: Maybe an alternative to **_Obj\_owner** could references for C, but only in function parameters.  
 
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


## Initialization and assignment
### Owner = Owner

In this situation we always move the ownership and **\_Move** is required.


```c  
_Owner T b;
_Owner T a = _Move b;
```

```c  
_Owner T b;
_Owner T a;
a = _Move b;
```

We have the ownership moved from b to a.

The exception of the usage of **\_Move** is when initializing from a function result.

```c
_Owner T make_owner();
_Owner T a = make_owner();
```
  
> Design note: The usage of _Move was not required in any way, by the type system or by the flow analysis. The reasoning was to make code more explicit.

### Owner = Non-owner
  
This is not allowed.  
The exception is we can initialize/assign owner pointer to the null pointer constant. 

```c
T _Owner * p1 = 0;       //OK
T _Owner * p2 = NULL;    //OK
T _Owner * p3 = nullptr; //OK
```

```c
T _Owner * p1;
p1 = 0; //OK
```

> Design note: This could be extended for non pointers. The problem is we need a extra annotation to define the "empty" value for non pointers.   


### Non-Owner = Owner  

In this situation we always have a view.

```c  
_Owner T b;
T a = b;            //OK
```

```c  
_Owner T b;
T a;
a = b;            //OK
```

We say "a is a view to b".

We cannot have a view for a object with the storage duration shorter than the view.

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
 _Owner T a;
  global = a; //ERROR
}
```

```c  

void f()  
{  
   T v;    
   {
      _Owner T a;
      v = a; //ERROR
   }
}
```  

(The last samples are not implemented yet in cake)

## Returning variables
  
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

Following the general rule that view object duration must be shorter than the owner object, returning a owner local storage variable is an error.

```c
T F()
{
    _Owner T local;
    return local; //ERROR    
} 
```

However, we can return non local variables including parameters 

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

We can use **\_Implicit**  to make the usage of **_Move** optional.   
  
This is useful when the semantics of the function is very clear, for instance if the name of the function is "destroy" "free" etc.  

```c
void x_destroy(_Implicit struct X * _Obj_owner);
```

(This can be changed to a attribute in the future)
  
**\_Move** can be used for instance, when inserting items at some container.
  
```c  
list_add(&list, _Move item);
```  
  
Or when the parameter will consume all the input.
  
```c  
p_new_items = _Move process(_Move p_old_items);
```  

### void F(Owner); F(Non-Owner);
This is not allowed.  
The exception is the null pointer constant.

```c
  F(0); //OK  
```

### void F(Non-Owner); F(Owner);


Following the general rule that view object duration must be shorter than the owner object, we cannot pass owner object return by a function.

```c
  F(make_owner()); //ERROR  
```
  
But we can pass variables, and the function parameter is a view of the argument.  

```c
  _Owner T a; 
  F(a); //OK (view)
```


### Type system samples


## Flow analysis

When owner objects goes out of scope, the flow analysis
must check if the object has been moved. 

###  Algorithm

Compare these two situations.

```c
void f(int condition)
{
    int* _Owner p = malloc(sizeof(int));
    if (condition)
        goto end;   
end:
   free(p);
}
```

```c
void f(int condition)
{
    int* _Owner p = malloc(sizeof(int));
    if (condition)
        goto end;
    free(p);
end:
}
```

The basic idea of the algorithm is that we need a clear path from the point where variables are moved until the point they leave scope.


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

### Sample 7

This sample shows how _View can be used to implement swap.

```c
/*    
  See also: http://thradams.com/cake/ownership.html
*/

void free(_Implicit void * _Owner p);

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

void person_destroy(_Implicit struct person * _Obj_owner p) {
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

### Sample 8


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

### Sample 9

Uncomment /\*_Owner\*/ in this sample and you will see chain reaction that requires changes to make sure this program is not leaking memory.

```c
/*  
  See also: http://thradams.com/cake/ownership.html
*/

char * /*_Owner*/ strdup(const char *s);
void free(_Implicit void * _Owner p);

struct X {
  char * text;
};

int main() {
   struct X x = {};
   x.text = strdup("a");
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



