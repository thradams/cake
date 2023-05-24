# Removing Phase 2

## Abstract

Phase 2 allows line continuation to be used at ANY position 
including slicing tokens.

For instance:

```c

#define M\
ACRO 1
MACRO 1

int main() {
   
   int a\
b = 1;
   ab= MACRO;

   a+\
+;
  /\
/ comment
}
```


This proposal aims to remove phase 2 considering line-continuation
as a blank token and allow it be used inside some tokens like
comments and literal strings.



## Current usage of line-continuation

The most common usage of line continuation is within preprocessor 
directives. It is also found in line comments, comments, and literal 
strings.

Except for preprocessor directives the other usages of line 
continuation inside tokens can be rewritten. 

Inside line comments

```c
 // line \
    comment
```

Can be replaced by

```c
 /* line 
    comment
 */
```

Inside literal strings

```
  "text \
continuation" 
```

Can be replaced by

```
  "text "
  "continuation" 
```

Inside comments

```
  /*comments \
comments*/
```

Can be replaced by

```
  /*comments 
comments*/
```

# Proposal

We propose line continuation to work as blank.
To match C++ 23, we also allow blanks after backslash and new-line.

To allow existing usage of line continuation inside 
string literal, comments and line-comments we suggest change:


```

line-continuation:
  \ blanks new-line

(6.4.5) string-literal:
  encoding-prefix opt " s-char-sequenceopt "

(6.4.5) s-char-sequence:
  s-char
  s-char-sequence s-char

(6.4.5) s-char:
   any member of the source character set except
   the double-quote ", backslash \, or new-line character
   escape-sequence
   line-continuation

 TODO comments
```

## Breaking Changes

```c
#define A B\
C
```

Instead of generating `BC`, it will generate `B C.` 

I conducted a search and found 0 cases of this usage. 
[Here is the search result](https://sourcegraph.com/search?q=context:global+%5Cw%5C%5C%5Cr%3F%5Cn%5Cw+lang:C+lang:C%2B%2B+count:2000&patternType=regexp&sm=0&groupBy=repo).

To prevent potential issues with this case, we can define that line 
continuation inside identifier tokens is an error. 




