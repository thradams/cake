### Preprocessor Options

`-I <dir>`
Add <dir> to the list of directories searched for include files. Same as GCC and MSVC.

`-D <macro>`
Define a preprocessing symbol for the source file. Same as GCC and MSVC.

`-E`  
Copy preprocessor output to standard output. Same as GCC and MSVC.

`-H`  
Output a list of include files used during compilation. Equivalent to /showIncludes in MSVC and -H in GCC.

`-dump-tokens`  
Output the token stream before preprocessing.

`-preprocess-def-macro`  
Expand and preprocess #define macros after expansion.

### Analysis Options

`-fanalyzer`  
Run Cake's built-in flow analysis pass, including ownership, nullability, and lifetime checks.

`-const-literal`  
Treat string literals as `const char[]` rather than `char[]`.

### Diagnostic Options

`-W<number>`  
Enable warning number `<number>`. See the [Warnings Reference](warnings.html).

`-Wno-<number>`  
Disable warning number `<number>`.

`-Wall`  
Enable all warnings.

`-disable-assert`  
Disable Cake's built-in `assert` statement extension and revert to standard macro behavior.

### Output Options

`-o` <name.c>  
Set the output filename when compiling a single file. Same as GCC and MSVC.

`-no-output`
Run all analysis passes but suppress file output.

`-line-directives`
Emit #line directives in the generated C89 output to preserve source location information.

`-sarif`  
Generate SARIF diagnostic output files. Compatible with the Microsoft SARIF Viewer extension for Visual Studio.

`-sarif-path <dir>`  
Specify the directory for SARIF output. Typical Visual Studio External Tools invocation:  
-Wstyle -msvc-output -no-output -sarif -sarif-path "$(SolutionDir).sarif" $(ItemPath)

`-msvc-output`
Format diagnostic output to be compatible with the Visual Studio IDE error parser.

`-fdiagnostics-color=never`  
Disable ANSI color codes in diagnostic output. Same as GCC.

### Style and Formatting Options

`-style=<name>`
Set the naming and formatting convention enforced by style warnings (-Wstyle / -w011).

Options:

`cake`

```c
void my_function(void)
{
    if (condition)
    {
        do_work();
    }
}
```

`gnu`

```C
void my_function(void)
{
    if (condition)
    {
        do_work();
    }
}
```

`microsoft`

```c
void MyFunction(void)
{
    if (condition)
    {
        DoWork();
    }
}
```

`llvm`
```c
void MyFunction(void) {
    if (condition) {
        doWork();
    }
}
```

`google`

```c
void MyFunction(void) {
    if (condition) {
        DoWork();
    }
}
```

`chromium`
```c
void MyFunction(void) {
    if (condition) {
        DoWork();
    }
}
```

`mozilla`

```c
void MyType::myFunction(void)
{
    if (condition) {
        doWork();
    }
}
```

`webkit`
```C
void MyType::myFunction(void)
{
    if (condition) {
        doWork();
    }
}
```
