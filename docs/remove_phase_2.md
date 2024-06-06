## Removing Phase 2

### Motivation

Phase 2, works as being a preprocessor of the preprocessor, it does not make any distinction or judgment
about where the backslash character `\` followed by a newline can be used. This allows it to be used in
nonsensical places like in the middle of identifiers, punctuations and `//single-line comment`.
I propose we remove Phase 2 and handle the backslash character `\` followed by a newline in Phase 3.

## Current Text of Phase 2

2. Each instance of a backslash character (\) immediately followed by a new-line character is
deleted, splicing physical source lines to form logical source lines. Only the last backslash on
any physical source line shall be eligible for being part of such a splice. A source file that is
not empty shall end in a new-line character, which shall not be immediately preceded by a
backslash character before any such splicing takes place.

## New text for phase 2

2. Phase 2, responsable for line splicing has been removed in C2Y.
 The backslash character (\) followed by new line are now handled at phase 3.

## 6.4 Lexical elements

I propouse the  backslash character (\) followed by spaces and new line to be handled inside 
string-literal, single line comment, comment and white spaces as follow.


Backslash character (\) followed optionally by spaces and new line are part of comment and single-line comments.

Sample:

```c
int main() {
  int i = 0;
  // See this path: c:\path\
  i = 2;
  return i; //always returns 2
}
```

```c
/*********\
  comment
\*********/
```

Backslash character (\) followed optionally by spaces and new line are ignored inside literal strings
Note: I will use · to represent the space character.

```c
const·char·s =
"a·\··
b";
```
Same as

```c
const·char·s·=·
"a·b";
```

Backslash character (\) followed optionally by spaces and new line are ignored inside whitespaces

```c
const char s = 1·+·\···
····2;
```
Same as 
```c
const char s = 1·+·····2;
```

## Breaking changes
For all other tokens like idenfifiers, keywords, punctuators, constant the backslash character (\) is not accepct.

Currently we have

```c
#define A\
B\
C
```
This is the same as
```c
#define ABC
```
But if we had one space before B. This would be the same as
```c
#define A BC
```
With this proposal we have a error while parcing identifer A.
This will promove clarification because the code will have to be write as
```c
#define AB \
C
```
or
```c
#define A B \
C
```
Slicing punctuators and other tokens will be an error.

```c
 int i;
 i+\
+;
```


## References

Trimming whitespaces before line splicing
https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2223r2.pdf








