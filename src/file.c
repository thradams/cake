
#if defined __x86_64__ && !defined __ILP32__
# define __WORDSIZE	64
#else
ERROR
#endif

# define UINT8_C(c)	c
# define UINT16_C(c)	c
# define UINT32_C(c)	c ## U
# if __WORDSIZE == 64
#  define UINT64_C(c)	c ## UL
# else
#  define UINT64_C(c)	c ## ULL
# endif

#ifndef SDL_UINT64_C
#if defined(UINT64_C)
#define SDL_UINT64_C(c)  UINT64_C(c)
#elif defined(_MSC_VER)
#define SDL_UINT64_C(c)  c ## ui64
#elif defined(__LP64__) || defined(_LP64)
int 
#define SDL_UINT64_C(c)  c ## UL
#else
#define SDL_UINT64_C(c)  c ## ULL
#endif
#endif

typedef unsigned long int Uint64;

union {
	Uint64 u64;
	double d;
} inf_union = {
	SDL_UINT64_C(0x7ff0000000000000)  /* Binary representation of a 64-bit infinite double (sign=0, exponent=2047, mantissa=0) */
};
