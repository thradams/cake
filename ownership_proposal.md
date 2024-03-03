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

TODO
