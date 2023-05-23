# Removing compiler phase 2

## Abstract 
 
Except in preprocessor, new-line does not have any syntax influence in C. 
In preprocessor however, the only way of writing multi line directives is using backslash-newline
that is processsed at phase 2. 
We sugest the modification of the language to remove phase 2 and handle line continuation
at preprocessor directives.



## PROBLEM DESCRIPTION 

After preprocessor line continuation is not necessary.
For instance

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

Becuse backslash-newline happens at phase 2 we also can make silly things
in preprocessor phase like this.

```c
#de\ 
fine M\ 
ACRO 1 

MACRO 
```

## Proposal

Remove phase 2.
The usage of line-continuation will be handled at preprocessor directives just
ignoring completely the token.

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


Or split macro directives 

```c
#de\
fine A B\
C
```

At preprocessor directes like # if , # define line continuation tokens will be handled at same
position as blanks, but diferently of blanks the line-continuation token will be deleted/ignored.

```c
#define M \
    {     \    
    }  

```

Inside contant expression it also will be ignored.

Sample
```c
#if defined A || \
    defined B 
#endif
```

The only quiet change is 
```c
#define A B\
C
//A will expand to B C instead of BC
```
I don't think we will find code like that is intentional.


