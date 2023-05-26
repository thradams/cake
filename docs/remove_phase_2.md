## Remove phase 2

## Proposal

 Except for literal-string, comment, and line comment, line-slicing in the middle
 of any token is an warning.
 
  ```c
 #define M\
 ACRO 1
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
compiler can say: note : you can use adjacents strings
 
warning line-slicing inside line-comments - deprecated

```c
    int a;
    \\comment\
    a = 1
 ```

warning about line-slicing other than inside comments, literal string, line comments
if used in preprocessor text-line diretive.
It is expected and normal find line-slicing in # directives.
```c
    #define X { \
    1, \
    2  \
    }
 ```
