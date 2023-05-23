### Proposal Category: Language simplification / modernization


# Removing Compiler Phase 2

## Abstract

In C, new-lines have no syntax influence except within the preprocessor. Within the preprocessor, multi-line directives are currently written using the backslash-newline combination, which is processed during phase 2.

We propose modifying the language by eliminating phase 2 and handling line continuation specifically within preprocessor directives where it is utilized.

## Problem Description

There are instances of unnecessary line continuation in C code that can be removed. For example:

```c
void F(int a, \
int b);
```
can be replaced with

```c
void F(int a,
         int b);
```

```c
const char S = "a \
  b";
```
can be replaced with

```c
const char S = "a"
               "b";
```

In more complex cases:

```c
int a\
b = 1;
int main() { 
  ab = 1;
}

//comment \
int b = 2;
```

None of these line continuations are necessary. (GCC issues a warning for line continuation in line comments.)

Another issue is that since backslash-newline occurs during phase 2, it allows breaking identifers, as shown in this example:

```c
#de\ 
fine M\ 
ACRO 1 

MACRO 
```

## Proposal

Our proposal is to remove phase 2 and handle line continuation within preprocessor directives as if they were spaces, 
with the difference that they would be ignored.

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

After this change, A will expand to B C instead of BC. While intentional examples like this may not be common, it's important to ensure code safety. To address this potential problem, we recommend implementing an error mechanism that detects when the line continuation splits two tokens that, when combined, would result in an identifier. In such cases, the programmer would be required to refactor the code and choose one of the following options:

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

## Samples

Inside a constant expression, line continuation will also be ignored. For example:

```c
#if defined A || \
    defined B 
#endif
```
## Design consideration
We could be more restritive about were line continuation can happen. For instance.

```
#undef \
  M
```
But in this case we weed specify the grammar in each preprocessor directive making the proposal more complicated.


## Implementation

This proposal has been implemented in Cake and is active in the current version. You can try it out at http://thradams.com/cake/playground.html.


