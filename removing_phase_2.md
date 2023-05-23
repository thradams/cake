### Proposal Category: Language simplification / modernization


# Removing Compiler Phase 2

## Abstract

In C, new-lines have no syntax influence except within the preprocessor.
Within the preprocessor, multi-line directives are currently written using the
backslash-newline combination, which is processed during phase 2.

We propose modifying the language by eliminating phase 2 and handling line continuation 
at the grammar level.

## Problem Description

Phase two is a lawless land. We can insert line-continuation in **ANY** place.

Beetwen punctuators

```
int main() {
  int a;
  a+\
+;
}
```

```
int main() {
  /\
/  line comments
}
```
In the middle of identifiers

```
int main() {
   int a\
b;
   ab = 1;
}
```
etc.


## Where it can be found in real programs?

 1 - Preprocessor directives
 
 ```c
 #define X \
  { \
    1, \
    2 \
  }
 ```
 
 2 - Line comments
 
 ```c
            //line comment \
              continuation of line comment
 ```
  
 3 - big literal Strings
 
Inside comments
```c
const char *s = "this is a big \
literal string";
```

## Proposal

Our proposal is to remove phase 2 and handle line continuation at grammar level.

## Breaking Changes

1 - Code that relies on line continuation surviving the preprocessor will result in an error. For example:

```c
int a\
b;
```

In this case, the line continuation needs to be deleted.

2 - Identifiers that were joined in phase 2 will now be treated as two separate identifiers instead of one. For example:

```c
#define A B\
C
```

After this change, A will expand to `B C` instead of `BC`.

While intentional examples like this may not be common, it's important to ensure code safety.
To address this potential problem, we recommend implementing an error mechanism that detects when the line continuation splits two tokens that, when combined, would result in an identifier. In such cases, the programmer would be required to refactor the code and choose one of the following options:

Maybe a simpler solution is just emmit an error if line continuation is used in identifers.

```c
#define A BC
// A will expand to BC
```
or 

```c
#define A B \
C
// A will expand to B C
```

3 - The following code will no longer compile:

```c
#de\
fine A B\
C
```

4 - spaces after backslash and before new-line (this is optional in this proposal)

```c
#define A \ spaces
C
```
Error before and now it works.

5 - line continuation inside comments

```c  
  // this is my \
     comment      
```
`error line-continuation cannot be used in line-comments`
It will have to be refactored to

```c  
  /* this is my
     comment   */
```
The objective is be explicty about the change.

Breaking changes is preprocessor are not something new. They did happen before.
Microsoft for instance rewrote the preprocessor causing breaking changes
https://learn.microsoft.com/en-us/cpp/preprocessor/preprocessor-experimental-overview?source=recommendations&view=msvc-170

Breaking changes described here are so far just a matter of clarification. 

- Do you really want to create a new identifier? If so please refactoring.
- Do you really want a line continuation inside comments? If so please refactoring.


## C and C++

It is expected C++ to implement the same proposal.

If it is implemented only in C then C is the common subset that works in both (after refactoring some usages).

## Samples

Inside a constant expression, line continuation will also be ignored. For example:

```c
#if defined A || \
    defined B 
#endif
```
## Design consideration
We could be more restritive about were line continuation can happen. For instance, it could not be allowed
after undef.

```c
#undef \
  M
```
But in this case we weed specify the grammar in each preprocessor directive making the proposal more complicated.


## Implementation

This proposal has been implemented in Cake and is active in the current version. You can try it out at http://thradams.com/cake/playground.html.


