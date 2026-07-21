## 12. GCC Compatibility

Cake recognizes the following GCC built-ins and extensions:

    - `__builtin_va_list`
    - `__builtin_va_start`
    - `__builtin_c23_va_start`
    - `__builtin_va_end`
    - `__builtin_va_arg`
    - `__builtin_va_copy`
    - `__builtin_offsetof` — same as Cake's built-in `offsetof`
    - `__attribute__`
    - `__typeof__` — alias for `typeof`

Additional GCC built-ins are declared in `src/include/x86_x64_gcc_builtins.h`.

Pre-defined GCC compatibility macros: https://gcc.gnu.org/onlinedocs/cpp/Predefined-Macros.html
