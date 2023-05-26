## Line-slicing rules

## Proposal

 Except for literal-string, comment, and line comment, line-slicing in the middle
 of any token is an warning.
 
  ```c
 #define M\
 ACRO 1
  
 int a\
 b;
 ```
 
 ```
 warning token-sliced
 ```
 
 line-slicing inside comments are handled as normal
 
 ```c
   /*
      path = C:\path\
   */
 ```
 
line-slice inside literal string are almost normal
 ```c
    const char* s = "ab\
 cd";
 ```
 ```
 note: you can use adjacents strings
 ```
 
warning line-slicing inside line-comments - deprecated

```c
    int a;
    \\comment\
    a = 1
 ```
 
 ```
 warning: line slicing inside line-comments are deprecated. use /*comments*/
 ```

warning about line-slicing other than inside comments, literal string, line comments
if used in preprocessor text-line diretive.

```c
    int a / 
    =  1;
 ```
 
 ```
 warning: unnecessary line-slicing
 ```

It is expected and normal find line-slicing in # directives.
```c
    #define X { \
    1, \
    2  \
    }
    #if 1 || \
        2
    #endif
    
    #undef \
           X
 ```
