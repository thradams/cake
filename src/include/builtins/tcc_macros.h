#define __STDC_VERSION__ 202311L
#define __TINYC__ 928
#define __SIZEOF_POINTER__ 8
#define __SIZEOF_INT__ 4
#define __INT_MAX__ 0x7fffffff
#define __SIZEOF_LONG_LONG__ 8
#define __LONG_LONG_MAX__ 0x7fffffffffffffffLL
#define __CHAR_BIT__ 8
#define __ORDER_LITTLE_ENDIAN__ 1234
#define __ORDER_BIG_ENDIAN__ 4321
#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#define __UINTPTR_TYPE__ unsigned __PTRDIFF_TYPE__
#define __INTPTR_TYPE__ __PTRDIFF_TYPE__
#define __INT32_TYPE__ int
#define __PRETTY_FUNCTION__ __FUNCTION__
#define _Nonnull
#define _Nullable
#define _Nullable_result
#define _Null_unspecified

#ifdef CAKE_TARGET_PLATFORM_WIN_X64
    #define __x86_64__ 1
    #define __x86_64 1
    #define __amd64__ 1
    #define _WIN32 1
    #define _WIN64 1
    #define __SIZEOF_LONG__ 4
    #define __SIZE_TYPE__ unsigned long long
    #define __PTRDIFF_TYPE__ long long
    #define __LLP64__ 1
    #define __INT64_TYPE__ long long
    #define __LONG_MAX__ 0x7fffffffL
    #define __WCHAR_TYPE__ unsigned short
    #define __WINT_TYPE__ unsigned short
    #define __declspec(x) __attribute__((x))
    #define __cdecl
#endif

#ifdef CAKE_TARGET_PLATFORM_LINUX_X64
    #define __STDC__ 1
    #define __STDC_HOSTED__ 1
    #define __x86_64__ 1
    #define __x86_64 1
    #define __amd64__ 1
    #define __linux__ 1
    #define __linux 1
    #define __unix__ 1
    #define __unix 1
    #define __LP64__ 1
    #define __SIZEOF_LONG__ 8
    #define __SIZE_TYPE__ unsigned long
    #define __PTRDIFF_TYPE__ long
    #define __INT64_TYPE__ long
    #define __LONG_MAX__ 0x7fffffffffffffffL
    #define __WCHAR_TYPE__ int
    #define __WINT_TYPE__ unsigned int
    #define __REDIRECT(name, proto, alias) name proto __asm__(#alias)
    #define __REDIRECT_NTH(name, proto, alias) name proto __asm__(#alias) __THROW
    #define __REDIRECT_NTHNL(name, proto, alias) name proto __asm__(#alias) __THROWNL
#endif

#ifdef CAKE_TARGET_PLATFORM_MACOS_ARM64
    #define __STDC__ 1
    #define __STDC_HOSTED__ 1
    #define __aarch64__ 1
    #define __arm64__ 1
    #define __AARCH64EL__ 1
    #define __APPLE__ 1
    #define __APPLE_CC__ 1
    #define __unix 1
    #define __unix__ 1
    #define __GNUC__ 4
    #define __LP64__ 1
    #define __SIZEOF_LONG__ 8
    #define __SIZE_TYPE__ unsigned long
    #define __PTRDIFF_TYPE__ long
    #define __INT64_TYPE__ long long
    #define __LONG_MAX__ 0x7fffffffffffffffL
    #define __LITTLE_ENDIAN__ 1
    #define __WCHAR_TYPE__ int
    #define __WINT_TYPE__ int
    #define __FINITE_MATH_ONLY__ 1
    #define __leading_underscore 1
    #define _DONT_USE_CTYPE_INLINE_ 1
    #define _FORTIFY_SOURCE 0
#endif
