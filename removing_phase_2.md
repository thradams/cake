# Removing Phase 2

## Abstract

This proposal aims to remove phase 2 from the C standard by creating grammar rules that describe the specific positions
where line continuation (backslash + new-line) occurs in real programs.

The current description of phase 2 allows line continuation to be used at ANY position, and it is important to emphasize this fact.
The objective of this proposal is to gather usage patterns of line continuation in the field and translate them into precise grammar rules.

## Current Usage Research

### 1 - Preprocessor
The most common usage of line continuation is within preprocessor directives, where new-line has a syntax significance.
Programmers use line continuation to write multi-line preprocessor directives.

```c
#define M(a)   (a && \
               1 )
```

### 2 - Line Comments

```c
 // line \
    comment
```

### 3 - Comments

```c
  /*								\
	 * Because of FreeBSD, we care about gcc 4.2, which doesn't have\
	 * an atomic exchange builtin.  We fake it with a CAS loop.	\
	 */								\

#define M \
	/*								\
	*/								\
  1\
}		
```

### 4 - Literal Strings

```c
   s = "\
Even if you've mastered the art of the cover letter and the resume, \
another part of the job search process can trip up an otherwise \
qualified candidate: the writing sample.\n\
\n";

```

Based on these cases, we can incorporate the following rules into the grammar:

- Backslash + new-line can occur inside literal strings, line comments, and comments.
- For preprocessor directives, line continuation is considered a blank token.

## Breaking Changes

```c
#define A B\
C
```

Instead of generating "BC," it will generate "B C." 

I conducted a search and found 0 cases of this usage. 
[Here is the search result](https://sourcegraph.com/search?q=context:global+%5Cw%5C%5C%5Cr%3F%5Cn%5Cw+lang:C+lang:C%2B%2B+count:2000&patternType=regexp&sm=0&groupBy=repo).

To prevent potential issues with this case, we can define that line continuation inside identifier tokens
is an error. 

So far all changes affect only tokens.


