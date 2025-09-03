

#define   __STDC_VERSION_FENV_H__ 

#define   FE_ALL_EXCEPT (0x20 | 0x04 | 0x10 | 0x08 | 0x01)
#define   FE_DIVBYZERO 0x04
#define   FE_INEXACT 0x20
#define   FE_INVALID 0x01
#define   FE_OVERFLOW 0x08
#define   FE_UNDERFLOW 0x10
#define   FE_DOWNWARD 0x400
//#define   FE_TONEARESTFROMZERO FE_TONEARESTFROMZERO
#define   FE_TONEAREST 0
#define   FE_TOWARDZERO 0xc00
#define   FE_UPWARD 0x800
#define   FE_DFL_ENV ((const fenv_t *) -1)
#define   FE_DFL_MODE ((const femode_t *) -1L)
//#define   FE_DEC_DOWNWARD FE_DEC_DOWNWARD
//#define   FE_DEC_TONEARESTFROMZERO FE_DEC_TONEARESTFROMZERO
//#define   FE_DEC_TONEAREST FE_DEC_TONEAREST
//#define   FE_DEC_TOWARDZERO FE_DEC_TOWARDZERO
//#define   FE_DEC_UPWARD FE_DEC_UPWARD
//#define   FE_SNANS_ALWAYS_SIGNAL FE_SNANS_ALWAYS_SIGNAL

typedef unsigned short int fexcept_t;

typedef struct
{
    unsigned short int __control_word;
    unsigned short int __glibc_reserved1;
    unsigned short int __status_word;
    unsigned short int __glibc_reserved2;
    unsigned short int __tags;
    unsigned short int __glibc_reserved3;
    unsigned int __eip;
    unsigned short int __cs_selector;
    unsigned int __opcode : 11;
    unsigned int __glibc_reserved4 : 5;
    unsigned int __data_offset;
    unsigned short int __data_selector;
    unsigned short int __glibc_reserved5;

    unsigned int __mxcsr;

}
fenv_t;

typedef struct
{
    unsigned short int __control_word;
    unsigned short int __glibc_reserved;
    unsigned int __mxcsr;
}
femode_t;

// functions
int feclearexcept(int excepts);
int fegetexceptflag(fexcept_t* flagp, int excepts);
int feraiseexcept(int excepts);
int fesetexcept(int excepts);
int fesetexceptflag(const fexcept_t* flagp, int excepts);
int fetestexceptflag(const fexcept_t* flagp, int excepts);
int fetestexcept(int excepts);
int fegetmode(femode_t* modep);
int fegetround(void);
int fesetmode(const femode_t* modep);
int fesetround(int rnd);
int fegetenv(fenv_t* envp);
int feholdexcept(fenv_t* envp);
int fesetenv(const fenv_t* envp);
int feupdateenv(const fenv_t* envp);
