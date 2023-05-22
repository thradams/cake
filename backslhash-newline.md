# Remove backslash-newline after preprocessor phase

## Abstract 
 
Except in preprocessor, new-line does not have any syntax influence in C. In preprocessor however, the only way of writing multi line source code is using backslash-newline. 
Using backslash-newline in other scenarios other than preprocessor can be silly, for instance inside line comments, macro names or identifiers. It also complicates tools and the standard. 
We propose to remove backslash-newline usage other than in preprocessor. 

 

## PROBLEM DESCRIPTION 

Let's take this sample 

```c
#de\ 
fine M\ 
ACRO 1 

// line comment \ 
MACRO 

int main() { 
    int a\ 
b; 
   ab = MACRO; 
} 
```

The complication starts with the editor. Many if not all editors may have problems with the syntax color of #define or line comment. They may be fixed. But it's worth? 
Imagine refactoring tool that does rename having to deal with the identifier ab. The standard also needs a phase (2) to describe the backlash-newline behavior. GCC have a warning when backlash-newline is used in line comments and MISRA C (maybe others) have a rule for it. 

Removing backlash-newline is a simplification and modernization of the language and makes the language safer and tools simpler. 
 
## Proposal

Create backlash-newline-token that works as space (blanks) in preprocessor but not in compiler phases.
## Breaking changes

This sample would not compile anymore because the compiler would see the backlash-newline-token
that is not allowed. So it not a quiet change.

```
int a\
b;
```

Here the backlash-newline-token words as space. 

```c
#define A B\
C
```
So A expands to B C instead of BC.
This is a quiet change and compilers could have a warning add more spaces around the 
backlash-newline-token or refactor to
```c
#define A BC
```
if this is the intention.

## Doc changes

```
"2. Each instance of a backslash character (\) immediately followed by a new-line character is 
deleted, splicing physical source lines to form logical source lines. Only the last backslash on 
any physical source line shall be eligible for being part of such a splice. A source file that is 
not empty shall end in a new-line character, which shall not be immediately preceded by a 
backslash character before any such splicing takes place. 
"

"(2) A nonempty source file does not end in a new-line character which is not immediately preceded 
by a backslash character or ends in a partial preprocessing token or comment (5.1.1.2) 
"
```

```
 Some note.
 In preprocessor backlash-newline-token works as blank.
```
