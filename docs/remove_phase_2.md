
## Removal of Phase 2

### Motivation

Phase 2 works as a preprocessor of the preprocessor, without making any distinction or judgment about where the backslash character `\` followed by a newline can be used. This allows it to appear in nonsensical places such as in the middle of keywords, identifiers, constants, punctuation, and at the end of single-line comments.

This complexity makes the language and tools more cumbersome and can create errors due to the misuse of whitespace.

I propose we remove Phase 2 and handle the backslash character `\` as part of lexical elements.

## Current Text of Phase 2

2. Each instance of a backslash character (\) immediately followed by a newline character is deleted, splicing physical source lines to form logical source lines. Only the last backslash on any physical source line shall be eligible for being part of such a splice. A source file that is not empty shall end in a newline character, which shall not be immediately preceded by a backslash character before any such splicing takes place.

## New Text for Phase 2

2. Phase 2, responsible for line splicing, has been removed in C2Y. Phase 2 is empty in C2Y.

## 6.4 Lexical Elements

I propose that the backslash character `\` followed optionally by  spaces and a newline be handled within string literals, single-line comments, multi-line comments, and whitespace as follows:

They will be part of comments

Sample:

```c
int main() {
  int i = 0;
  // See this path: c:\path\
  i = 2;
  return i; // always returns 2
}
```

```c
/*********\
  comment
\*********/
```

They are parsed and ignored inside string literals

```c
const·char·s·=·
"a·\··
b";
```
is the same as

```c
const·char·s·=·
"a·b";
```

They are parsed and ignored inside white spaces

```c
const char s = 1·+·\···
····2;
```
is the same as 
```c
const char s = 1·+·····2;
```

## Breaking Changes
For all other tokens like identifiers, keywords, and punctuators, the backslash character (`\`) is not accepted.

Currently, we have

```c
#define A\
B\
C
```
This is the same as
```c
#define ABC
```
But if we had one space before B, this would be the same as

```c
#define A BC
```
With this proposal, we will have an error while parsing the identifier A. This will promote clarification because the code will have to be written as
```c
#define AB \
C
```
or
```c
#define A B \
C
```
Slicing punctuators and other tokens will result in an error.

```c
int i;
i+\
+;
```

## References

Trimming whitespace before line splicing:
[https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2223r2.pdf](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2223r2.pdf)




