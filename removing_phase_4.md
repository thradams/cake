# Removing Phase 4

## Abstract

This proposal aims to remove phase 4 from the C standard by creating grammar rules that describe the specific positions
where preprocessor directives and macro expansion can occurs following the convention already used in real programs.

The current description of phase 4 allows line continuation to be used at  ANY line position, and it is important to emphasize this fact.
Because the phases are diferents preprocessor directives dont have access of compiler information. This can be useful.

The objective of this proposal is to gather usage patterns of preprocessor directives in the field and translate them into precise grammar rules
at compiler phases.

Macro invocations also will happen at compiler phase allowing checks.

## Current Usage Research, preprocessor directives and macro expansion

### 1 -  #define/#undef
The most common usage of preprocessor directives is file scope.
- file scope
- function scope
- inside structs?
- 
### 2 -  #error # warning
- file scope
- function scope
- inside structs

### pragma 
- file scope
- function scope

### if groups
- file scope
- function scope
- inside structs 

the usage of preprocessor directives are similar os where static_assert can be used.

### 3 - macro expansion

statements

```c
 IF_ERROR_RETURN;
 FOR_EACH( )
```

Expressions

constants and function like macros

```c
  int x = C;
```

labels

```c

  ONERROR

```


keywords
```c
  INLINE
```

declarations
```c
  DECLARE(X)
```

attributes
```c
  NO_DISCARD
```



## Breaking Changes
