# New qualifiers to enable checked lifetime contracts

## Abstract

This proposal introduces new qualifiers to improve safety regarding the misuse of object lifetimes.
These new qualifiers add contracts that can be checked at compile time, but requiring flow analysis.


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

We introduce several qualifiers called ownership qualifiers.

Just like `const` has in its basic principle avoid the object to be modified, 
ownership qualifiers have principles in mind and all the principles serve to keep the objective.

Principles:

 1 - Each object has only one owner
 2 - Ownership can be transferred between owners
 3 - Before the end of its lifetime, the owner object must transfer the ownership of the object it owns.
 4 - The object referenced by non-owner (view) must have a valid lifetime.

This principles serve as constrains to archive safety and avoid undefined behavior.

This part is from C23 standard.

"
6.2.4 Storage durations of objects 
...If an object is referred to outside of its lifetime, the behavior is undefined.
If a pointer value is used in an evaluation after the object the pointer points to (or just past) reaches
the end of its lifetime, the behavior is undefined.
The representation of a pointer object becomes indeterminate when the object the pointer points to 
(or just past) reaches the end of its lifetime.
"

## Ownership qualifiers

`_Owner` indicates that an object works as a reference to another object, 
managing its lifetime as its unique owner. When _Owner qualifier are aplied to pointes
it means the pointer is the owner of two objects.

`_Obj_owner` It is a qualifier for pointer that indicates the pointer is the owner 
of the pointed object but it not owner of the memory.

`_View` qualifier is the opposite of _Owner.

`_Opt` Used in function arguments to indicate the argument can be uninitialized.


## Constrains that don't require flow analysis

The result of a function returning a owner object cannot be discarded.

```c
int* _Owner f();
int main(){
 f(); //error discarding an owner object
}
```

The result of a function returning a owner object must be transferred to another owner object.

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

The result of a function returning a owner object cannot be returned as non-owner.
Except:
 - If the storage duration is static, or function pointer
 
 ```c
_Owner int i;
int* f()
{
   return i; //ok
}
```

- If the owner returned is a function argument.
 
 ```c
int* f(int * _Owner p)
{
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

- A _Obj_Owner qualified object can assigned to owner object.



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




