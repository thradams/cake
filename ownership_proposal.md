# New qualifiers to enable lifetime memory safety

## Abstract

This proposal introduces new qualifiers to improve safety regarding the misuse of object lifetimes.
These new qualifiers creates contracts that can be checked at compile time.


## Introduction, Motivation

The motivation is to check at compile time the correct usage of patterns that already exist in C.
For instance, how could we check at compile time the correct usage of `fopen` and `fclose`.

```c
#include <stdio.h>
int main()
{
  FILE * f = fopen("file.txt", "r"); 
  if (f)
    fclose(f);
}
```


## Design

We introduce several qualifiers to established new compile time contracts.

Principles for lifetime contracts

 1. Each object has only one owner
 2.  Ownership can be transferred between owners
 3. Before the end of its lifetime, the owner object must transfer the ownership of the object it owns.
 4. The object referenced by non-owner (view) must have a valid lifetime.


## Ownership qualifiers

`_Owner` An owner qualified object is an object referencing another object and managing its lifetime. When _Owner qualifier is applied to pointer it means the pointer if the owner of the memory and the object.

`_Obj_owner` It is a qualifier for pointer only that indicates the pointer is the owner 
of the pointed object but it not owner of the memory.

`_View` Qualifier is the opposite of _Owner.

`_Opt` Used in function arguments, for pointer only, to indicate the argument can be null. 
  
`_Out` Indicates the object is uninitialized


## Constrains

- The result of a function returning a owner object cannot be discarded.

```c
int* _Owner f();
int main(){
 f(); //error discarding an owner object
}
```

- The result of a function returning a owner object must be transferred to another owner object.

```c
int* f();
int main(){
 int * p = f(); //error
}
```

```c
int* f();
int main(){
 int * _Owner p = f(); //ok
}
```

- The result of a function returning a owner object cannot be returned as non-owner, except if the storage duration is static, or function or if returns an argument.
 
```c
_Owner int i;
int* f() {
	return i; //ok
}
int* f(int * _Owner p) {
   return p;
}
```


- A view object cannot be passed to a owner argument

```c
void fclose(_Owner int i);
int* f()
{
   int i;
   fclose(i); //error
}
```

 - The result of a function returning an owner object cannot be passed to a view parameter.
 
```c
void free(void* p);
void * _Owner make();
int main()
{
    free(make()); //error
}
 ```
 
- A _Obj_Owner qualified object can assigned to owner object.

```c
void delete(void* _Owner p);
void destroy(void* _Obj_owner p)
{
    delete(p); //error
}
```
 
# Semantics with flow analysis
- Assignment of owner objects move the ownership.
- Assignment of non-owner to owner creates a view reference
- Assignment of owner pointer to _\Obj\_owner transfer the ownership of the object but not the memory

## Flow analysis constrains

- Only initialized objects can be passed to _Opt arguments

Already moved to function because initialized. So they cannot be used for read.

Before assignment of owner object the object cannot be owning resources

Assignment transfer the ownership

Function arguments cannot leva the function scope with uninitialized state

Before the end of its lifetime owner objects must have moved the object they own

Every function argument is considered initialized except if _Opt qualifier is used.]

## Code transition

TODO explain `ownerhip.h` strategy inspired by stdbool.h.
macros owner etc.

## Changes in standard




