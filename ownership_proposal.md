# New qualifiers to enable checked lifetime contracts

## Abstract

This proposal introduces new qualifiers to improve 
safety regarding the misuse of object lifetimes. These new qualifiers add contracts 
that can be checked at compile time.

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
Ownership qualifiers have principles in mind and all the rules serve to keep the objective.

Principles:

1 - Each object has only one owner
2 - Ownership can be transferred between owners.
3 - Before the end of its lifetime, the owner object must transfer the ownership of the object it owns.
4 - The object referenced by non-owner (view) must have a valid lifetime.

Some principles are not new. At C standard 

"6.2.4 Storage durations of objects "...If an object is referred to outside of its lifetime, the behavior is undefined.
If a pointer value is used in an evaluation after the object the pointer points to (or just past) reaches
the end of its lifetime, the behavior is undefined. The representation of a pointer object becomes
indeterminate when the object the pointer points to (or just past) reaches the end of its lifetime."


Lets introduce the first qualifier `_Owner`.

`_Owner` indicates that an object works as a reference to another object, 
managing its lifetime as its unique owner.

So here is the first rule.

Constrain 1 - The result of a function returning a owner object cannot be discarded.

```c
int* f();
int main(){
 f();
}
```

This rule is necessary to ensure the principle number 3.
"Before the end of its lifetime, the owner object must transfer the ownership of the object it owns."

And to ensure the same principle 

Constrain 2 - The result of a function returning a owner object must be transferred 
to another owner object.

```c
int* f();
int main(){
 int * p = f(); //warning/error
}
```

```c
int* f();
int main(){
 int * _Owner p = f(); //ok
}
```

Constrain 3 - We cannot return owner object as non-owner except:
 - If the storage duration is static
 - If the owner returned is a function argument


```c
_Owner int i;
int* f()
{
   return i; //ok
}
```
 

```c
int* f()
{
   _Owner int i;
   return i; //error
}
```
]TODO

## Code transition
TODO explain `ownerhip.h` strategy inspired by stdbool.h.
macros owner etc.

## Changes in standard




