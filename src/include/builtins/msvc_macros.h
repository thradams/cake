#define _CONSOLE
#define _WIN32 1
#define _INTEGRAL_MAX_BITS 64
#define _MSC_VER 1944
#define _MSC_EXTENSIONS 1
#define __pragma(a)

#ifdef CAKE_TARGET_PLATFORM_WIN_X86
    #define __STDC_NO_COMPLEX__  __STDC_NO_COMPLEX__
    #define __STDC_NO_THREADS__   __STDC_NO_THREADS__
    #define __STDC_NO_VLA__    __STDC_NO_VLA__
    #define _M_IX86 600
    void *__builtin_alloca (size_t __size);
#endif

#ifdef CAKE_TARGET_PLATFORM_WIN_X64
    #define __STDC_VERSION__ 202311L
    #define _WIN64 1
    #define _M_X64 100
    #define _M_AMD64 100
#endif
