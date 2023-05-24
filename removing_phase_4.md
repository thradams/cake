# Removing Phase 4

## Abstract

This proposal aims to remove phase 4 from the C standard by creating grammar rules that describe the specific positions
where preprocessor directives can occurs following the convention already used in real programs.

The current description of phase 4 allows line continuation to be used at  ANY line position, and it is important to emphasize this fact.
Because the phases are diferents preprocessor directives dont have access of compiler information. This can be useful.

The objective of this proposal is to gather usage patterns of preprocessor directives in the field and translate them into precise grammar rules
at compiler phases.

Macro invocations also will happen at compiler phase allowing checks.

## Current Usage Research

### 1 - preprocessor directives - file scope
The most common usage of preprocessor directives is file scope.

```c
#define M(a)   (a && \
               1 )
```
  
### 2 - macro expansion

statements

```c
 IF_ERROR_RETURN;
```

expressions

```c
 MAX(a, b);
 int x = CONSTANT;
```

keywords
```c
  INLINE
```

declarations
```c
  DECLARE(X)
```



## Breaking Changes

