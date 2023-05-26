## Line-Slicing Rules

## Proposal

With the exception of literal strings, comments, and line comments, line-slicing in the middle of any
token is considered a warning/deprecation.

```c
#define M\
ACRO 1

int a\
b;

#def\
ine X

```

```
warning: token-slicing
```

Line-slicing inside /*comments*/ is fine

```c

/*
**             -----------
**             | Parent  |
**             -----------
**            /     |     \
**           /      |      \
**  ---------   ---------   ---------
**  |Child-A|   |Child-B|   |Child-C|
**  ---------   ---------   ---------
*/

/****************************\
*                            *
*                            *
\****************************/

/*
   path = C:\path\
*/
```

Line-slicing inside "literal strings" are almost normal.

```c
const char* s = "ab\
cd";
```

```
note: you can use adjacent strings
```

Line-slicing inside //comments or backslash-new-line at the end of
//comment is warning deprecation.

```c
int main(){
    int a;
   //path= c:\Program Files\
   a = 1;
}
```

```
warning: multi-line \\comment is deprecated. use /*comments*/
```
gcc/clang already have a warning:

```
warning: multi-line comment [-Wcomment]
    3 |    //path= c:\Program Files\
      |    ^
```

Line-slicing is unnecessary when used outside of comments,
literal strings, or line comments, in preprocessor text-line
directives we have a warning.

```c
#if WINDOWS
WINBASEAPI HANDLE WINAPI CreateFileMappingFromApp(HANDLE, \
        LPSECURITY_ATTRIBUTES, ULONG, ULONG64, LPCWSTR);
#endif


int main() {
int a / 
=  1;

if (condition)
{
  {
                                                       \
  }
  function(context, (char*)zz, z, free, FLAG_UTF8);
}

```

```
warning: unnecessary line-slicing deprecated
```

line-slicing is expected inside # directives.

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

TODO accept spaces betewwen backslash and new-line (as in C++).

## Why deprecation?

Code that compiles with no warnings acording with these rules
especially about token slice, is prepared for the future if phase 2
is removed following these rules.

 - backslash-new-line is a blank token
 - backslash-new-line is handled inside literal strings and comments
 - backslash-new-line is not handled (error) inside //comments 
 
Compiler can warning (unnecessary) if backslash-new-line - that now wors
as blank - is used in preprocessor text-line.

This change would break code like this and other token-sliced samples.

```c
#define A B\
C
```
A would expand to `B C` instead `BC`.

List of tools not prepared to handle sliced tokens
 - Visual Studio Code - syntax color and IDE language server
 - Visual Studio syntax color, rename tool
 - ...
 








