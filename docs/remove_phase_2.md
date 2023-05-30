## Removing Phase 2

### Motivation

Phase 2, works as being a preprocessor of the preprocessor, it does not make any distinction or judgment
about where the backslash character `\` followed by a newline can be used. This allows it to be used in
nonsensical places like in the middle of identifiers, punctuations and `//single-line comment`.
We propose removing Phase 2 and handling the backslash character `\` followed by a newline in Phase 3.


## Phase 3 (modified)

The source file is decomposed into preprocessing tokens and sequences of white-space
characters (including comments).

_While parsing comments, white-spaces and literal strings a backslash character `\` immediately followed by a newline 
can be skipped as if non-exitent. For other types of tokens, such as identifiers, it is considered an error.

The usage of backslash-new-line inside of `//single-line comment` is deprecated. (GCC, CLANG and MSVC already have a warning for this)_

## Breaking changes

Code that slices tokens other than `literal strings`, `/*comments*/` or `//single-line comment` will result in an error.

```c
#define A\
B\
C
```

This code is easy to fix by promoting a clarification and not being dependent on where whitespace is used.
 
Ideally backslash character `\` used in `//single-line comment` should be part of the comment.

This is the behavior of any other language (except C and C++)  that uses `//single-line comment` . 

Unfortunatly some existing C++ and C code uses backslash-newline on purpouse to split the comment 
and this could be dangerous making the next line that before was a comment to be valid. Deprecation
will give time to remove \ from single-line comments.
When \ is intented to be part of the comment we can add a extra space after \. The problem here is
that C++ already make the spaces after \ and before new-line valid.

//TODO C++ comparion  (allow spaces beetwen backslash-newline  )








