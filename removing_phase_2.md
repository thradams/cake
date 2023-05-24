
# Removing Phase 2

## Abstract

This proposal ains to remove phase 2 from the C standard
creating grammar rules to describe where line continuation (backlash + new-line)
ocours in real programs.

The way phase 2 is described, line continuation can be at ANY
position. ANY position must be emphatised.

The objetive of this proposal is collect field usage of line-continuation
and translate to grammar rules.


## Current usage research.

### 1 - Preprocessor
This is the most obvious usage because the preprocessor directives is the only place 
where new-line have a syntax usage. So programmers uses line-continuation to
be able to write multi line preprocessor directives.

```c
#define M(a)   (a && \
               1 )

```

```c
#define M(a)   (a && \
               1 )

```


### 2 - Line comments


```c
 // line \
    comment
```

### 3 - comments

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



### 4 - Literal strings


```c
   s = "\
Even if you've mastered the art of the cover letter and the resume, \
another part of the job search process can trip up an otherwise \
qualified candidate: the writing sample.\n\
\n\";

```


Considering these case we have, to incorporate this into grammar we can:

define backlash+newline can ocorrus inside literal strings, line commments, commments.
For preprocessor directives line continuation is considered a blank token.

## Breaking changes

```c
#define A B\
C
```

Instead of generating BC it will generate B C.
I searched for this and found 0 cases.
https://sourcegraph.com/search?q=context:global+%5Cw%5C%5C%5Cr%3F%5Cn%5Cw+lang:C+lang:C%2B%2B+count:2000&patternType=regexp&sm=0&groupBy=repo

To protect this case we can define that line continuation inside identifier tokens is an error.





