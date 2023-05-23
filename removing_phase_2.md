# Removing compiler phase 2

## Abstract 
 
Except in preprocessor, new-line does not have any syntax influence in C. 
In preprocessor however, the only way of writing multi line directives is using backslash-newline
that is processsed at phase 2. We sugest the modification of the language to allow only
the reasonable use of line contination.




## PROBLEM DESCRIPTION 


Some samples of backslash-newline that survives preprocessor

```c

   void F(int a, \
int b);
  
const char S = "a \
  b";

 int a\
b = 1;
int main() { 
 ab = 1;
}

//comment \
int b = 2;
```

None of these usages are necessary.
Making a resarch where backslash-newline is used inside windows headers
100% usage are in preprocessor directives.

Becuse backslash-newline happens at phase 2 we also can make silly things
in preprocessor phase like this.

```c
#de\ 
fine M\ 
ACRO 1 

MACRO 
```
Searhing in windows headers I dind´t find any usage like this.

## Proposal

Remove phase 2. The usage of line-continuation will be handled at preprocessor directives.

## Breaking changes

Code using line continuation that survives preprocessor will result in error.

Sample:
```
int a\
b;
```

line-continuation needs to be deleted in this case.

Preprocessor that joins identifiers will break in two

Sample
```c
#define A B\
C
```
After this change A will expand to B C instead of BC.

!This section is incomplete!

```c

 # if constant-expression new-line groupopt
 etc..
 
 /*
 constant-expression can have line-continuation_opt  
 */

identifier-list:
  line-continuation_opt identifier 
  identifier-list line-continuation_opt , line-continuation_opt identifier

replacement-list:
  pp-tokens-opt
  line-continuation replacement-list
  
# define identifier                                replacement-list new-line
# define identifier lparen identifier-list_opt )    replacement-list new-line
# define identifier lparen ... )                   replacement-list new-line
# define identifier lparen identifier-list , ... ) replacement-list new-line

```



