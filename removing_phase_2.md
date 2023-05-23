
# Removing Compiler Phase 2

## Abstract

This proposal ains to remove phase 2 from the C standard
creating grammar rules to describe where line continuation can happen.

Line continution can be inserted in ANY place. But we want to search
where programmers are using it and remove silly usages like spliting 
punctuator in two partc etc.

## Current usage research.

1 - Preprocessor
This is the most obvious usage because the preprocessor directives is the only place 
where new-line have a syntax usage. So programmers uses line-continuation to
be able to write multi line preprocessor directives.

```c


```

2 - Line comments


```c


```

3 - Literal strings


```c


```




