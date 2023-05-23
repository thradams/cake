# Removing compiler phase 2

## Abstract 
 
In C, new-lines do not have any syntax influence except within the preprocessor. 
In the preprocessor, the only way to write multi-line directives is by using the 
backslash-newline combination, which is processed during phase 2.

We propose modifying the language by eliminating phase 2 and instead handling line continuation
specifically within preprocessor directives where it is utilized.


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

identifiers that were join in phase two now will be two identifiers instead of one

Sample

```c
#define A B\
C
```
After this change A will expand to B C instead of BC.

This will not compile

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

To avoid this problem we sugest an error if the line-continuation 
splits two tokens that tokegether would result in an identifier.
Then the programmer has to refactoring the code having two options

```c
#define A BC
//A will expand to BC
```
or 
```c
#define A B \
C
//A will expand to B C
```

This proposal have been implemented in cake and is active at current version.
http://thradams.com/cake/playground.html





