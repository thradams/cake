## Line-Slicing Rules

## Proposal

With the exception of literal strings, comments, and line comments, line-slicing in the middle of any
token is considered a warning.

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

Line-slicing inside comments is treated as normal.

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

Line-slicing inside literal strings are almost normal.

```c
const char* s = "ab\
cd";
```

```
note: you can use adjacent strings
```

Line-slicing inside line comments is deprecated.

```c
int main(){
    int a;
   //path= c:\Program Files\
   a = 1;
}
```

```
warning: line slicing inside line-comments is deprecated. use /*comments*/
```
gcc/clang already have a warning:

```
warning: multi-line comment [-Wcomment]
    3 |    //path= c:\Program Files\
      |    ^
```

Line-slicing is unnecessary when used outside of comments,
literal strings, or line comments, in preprocessor text-line directives
we have a warning.

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

It is expected and normal to encounter line-slicing in # directives.

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

## Design alternative

Code that compiles with no warnings acording with these rules especially
about token slice, is prepared for the future if phase 2 is removed making
backslash-new-line a blank token.

This change would break code like this

```c
#define A B\
C
```

A would expand to `B C` instead `BC` as it is today
(that is not used but..)


Even making backslash-new-line a blank token we could have a warning
in text-line directives.

Also removing phase 2 the backslash-new-line would be handled at
comments, line comments and literal strings. 

The warning in line comments would remain except unless we break 
compatibility.







