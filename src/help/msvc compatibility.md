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

*End of Cake C Compiler Reference Manual*