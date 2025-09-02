#ifdef _WIN32

        #define _ADDRESSOF(v) (&(v))
        typedef char* va_list;

        #if defined _M_IX86 && !defined _M_HYBRID_X86_ARM64

            #define _INTSIZEOF(n)          ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))

            #define __crt_va_start_a(ap, v) ((void)(ap = (va_list)_ADDRESSOF(v) + _INTSIZEOF(v)))
            #define __crt_va_arg(ap, t)     (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
            #define __crt_va_end(ap)        ((void)(ap = (va_list)0))

        #elif defined _M_X64

            void __cdecl __va_start(va_list*, ...);

            #define __crt_va_start_a(ap, x) ((void)(__va_start(&ap, x)))
            #define __crt_va_arg(ap, t)                                               \
                    ((sizeof(t) > sizeof(__int64) || (sizeof(t) & (sizeof(t) - 1)) != 0) \
                        ? **(t**)((ap += sizeof(__int64)) - sizeof(__int64))             \
                        :  *(t* )((ap += sizeof(__int64)) - sizeof(__int64)))
            #define __crt_va_end(ap)        ((void)(ap = (va_list)0))

        #else
             #error platform not defined
        #endif


        #define __crt_va_start(ap, x) __crt_va_start_a(ap, x)

        #define va_start __crt_va_start
        #define va_arg   __crt_va_arg
        #define va_end   __crt_va_end
        #define va_copy(destination, source) ((destination) = (source))


#endif
