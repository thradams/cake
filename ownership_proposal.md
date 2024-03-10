# New qualifiers to enable lifetime memory safety

## Abstract

This proposal introduces new qualifiers to improve safety regarding the misuse of object lifetimes.
These new qualifiers creates contracts that can be checked at compile time.


## Introduction, Motivation

## Design

We introduce several qualifiers to established new compile time contracts.

Principles for lifetime contracts

 1. Each object has only one owner
 2. Ownership can be transferred between owners
 3. Before the end of its lifetime, the owner object must transfer the ownership of the object it owns.
 4. The object referenced by non-owner (view) must have a valid lifetime.


## Ownership qualifiers


### _Owner
An owner qualified object is an object referencing another object and managing its lifetime.
When applied to pointer, it means the pointer is the owner of the pointed object and the owner of the 
storage of the pointed object.

### \_Obj\_owner
A pointer that is the owner of the pointed object but not the owner of the memory the pointed
object lives.

### _View
Qualifier is the opposite of _Owner.

### _Opt
Indicates that a pointer passed as function argument can be on null state.

### _Nonnull
Indicates that a pointer passed as function argument cannot be on null state.
  
### _Out
Indicates the object is uninitialized;

### _Zero
Indicates the object is all zeroes;



For structs, having at least one owner object makes the struct owner.

## Constrains and semantics



Constrain -The result of a function returning a owner object cannot be discarded.

```c
int* _Owner f();
int main(){
 f(); //error discarding an owner object
}
```

Constrain - The result of a function returning a owner object must be transferred to another owner object.

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

```c
void free(void* p);
void * _Owner make();
int main()
{
    free(make()); //error
}
```

Constrain - The result of a function returning a owner object cannot be returned as non-owner,
except if the storage duration is static, or if returns an argument.
 
```c
_Owner int i;
int* f() {
	return i; //ok i is static
}

int* f(int * _Owner p) {
   return p; //ok returning argument
}

int* f() {
   _Owner int* p = ...;
   return p; //error
}
```


Constrain - A view object cannot be passed to a owner argument

```c
void fclose(_Owner int i);
int* f()
{
   int i;
   fclose(i); //error
}
```

Constrain - \_Obj\_Owner should be used only in function parameters. (Maybe relaxed?)

  
Constrain - A \_Obj\_Owner qualified object cannot assigned to owner object or be passed as argument in a owner parameter.

```c
void delete(void* _Owner p);
void destroy(void* _Obj_owner p)
{
    void* _Owner p2 = p; //error
    delete(p); //error
}
```


Constrain - _Opt should be used only in function parameters.

Constrain - _Notnull should be used only in function return type or arguments.

```c
struct X
{
   char * _Notnull;
}
```


Semantic - When a owner pointer is assigned to other, the ownership is transferred.

The compiler should emit a diagnostic if the owner object designated by the left operand 
of assignment is referencing an object or possibly referencing and object.

```c
char * _Owner p1 = malloc();
char * _Owner p2 = malloc();
 p1 = p2; //warning p1 can be not null
```

The compiler should emit a diagnostic if a pointer passed to a _Nonnull qualified pointer 
is null or possibly null.

```c
void f(char * _Nonnull p);
char * _Owner p1 = malloc();
 f(p1); //warning p1 can be null but the argument is _Nonnull
```

The compiler should emit a diagnostic if an object passed as pointer argument leaves the
scope with any uninitialized or moved object.

```c
void f(struct X * p)
{
  free(p->name);
} //warning leaving scope with uninitialize p->nae
```

The compiler should emit a diagnostic if an object returned in a function is 
uninitialized or moved.

```c
struct X* _Owner f()
{
  struct X * p = ...
  delete_x(p);
  return p;
} //warning returning uninitialize p->name
```

The compiler should emit a diagnostic if an object returned in a function is 
uninitialized or moved.

```c
struct X* _Owner f()
{
  struct X * p = ...
  delete_x(p);
  return p;
} //warning returning uninitialize p->name
```

Semantics - All return object from function are considered initialized except if _Out qualifier is used;

```c
_Out void * malloc();
```

Semantics - All function arguments are considered initialized except if _Out qualifier is used;

```c
void init(_Out struct X *p);
```

If the argument is not initialized and the function is not _Out then the compiler needs a diagnostic.


Semantics  - When owner pointer are transferred to void *, only the memory of the object is transferred.
The pointed object must have been moved moved. or be null.


## Code transition

TODO explain `ownerhip.h` strategy inspired by stdbool.h.
macros owner etc.

## Changes in standard




