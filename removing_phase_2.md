# Removing compiler phase 2

## Abstract 
 
Except in preprocessor, new-line does not have any syntax influence in C.

In preprocessor however, the only way of writing multi line directives is 
using backslash-newline that is processed at phase 2. 

We suggest the modification of the language to remove phase 2 and handle line 
continuation at preprocessor directives - where it is used.


## PROBLEM DESCRIPTION 

Samples of unnecessary line continuation

```c
   void F(int a, \
int b);
```
can be replaced by

```c
   void F(int a,
          int b);
```

```c
const char S = "a \
  b";
```
Can be replaced by

```c
const char S = "a"
               "b";
```
and worst cases

```c
 int a\
b = 1;
int main() { 
 ab = 1;
}

//comment \
int b = 2;

```

None of these usages are necessary.
(gcc has a warning for line-continuation in line comments)

Because backslash-newline happens at phase 2 we also can make silly things
in preprocessor phase like this.

```c
#de\ 
fine M\ 
ACRO 1 

MACRO 
```

## Proposal

Remove phase 2 and handle line continuation at preprocessor directives 
as if they were spaces with the diference they are ignored/deleted.

## Breaking changes

Code using line continuation that survives preprocessor will result in error.

Sample:

```c
int a\
b;
```

line-continuation needs to be deleted in this case.

Preprocessor that joins identifiers will break in two identifiers

Sample

```c
#define A B\
C
```

After this change A will expand to B C instead of BC.


This silly sample will not compile anymore.

```c
#de\
fine A B\
C
```

Inside constant expression it also will be ignored.

Sample
```c
#if defined A || \
    defined B 
#endif
```


The change worries me more is:

```c
#define A B\
C
//A will expand to B C instead of BC
```
But I don't think we can find intentional samples like this.



