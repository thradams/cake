
---

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

---

## 13. MSVC Compatibility

Cake recognizes the following MSVC extensions:

- `__ptr32`, `__ptr64`
- `__int8` through `__int64` — fixed-width integer types
- `__declspec`
- `__cdecl`
- `__fastcall`, `__stdcall` — calling conventions
- `__forceinline` — alias for `inline` in Cake
- `__pragma`
- `__unaligned`

Pre-defined MSVC compatibility macros: https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros

See also: `src/include/x86_msvc_macros.h`, `src/include/x64_msvc_macros.h`

---