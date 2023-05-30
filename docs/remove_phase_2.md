## Removing phase 2

### Motivation

Phase 2 is preprocessor of the preprocessor and does not maky any distinction or judment 
where backslash character \ new line can be used. This allows it to be used in non sense
places like in the middle of identifers, punctuares and //comments.

GCC and clang have a warning when backslash character \ new line  is used 
inside //comments because the next line can be comment by accident.

We propose remove phase 2 and handle backslash character \ followed of new line
in phase 3.


## Phase 3

The source file is decomposed into preprocessing tokens 7) and sequences of white-space
characters (including comments). 

_**During token creation and the parsing of comments and white-spaces, a backslash 
character \ immediately followed by a new-line can be skipped if the token is a 
comment, line-comment, literal string, or white-space. However, for other types 
of tokens, such as identifiers, it is considered an error. 
The usage of backslash-new-line inside of line-comment is deprecated.**_

....

## Breaking changes
 
Code that slice tokens other than literal strings, /*comments*/ or //comment will
result in error

```c
#define A\
B\
C
```

This code and easy to fix promoting  a clarification and not begin
dependent where whitespace is used.



