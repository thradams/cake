
typedef typeof(sizeof(1)) size_t;


/*
  https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html
*/
bool __builtin_add_overflow(/*type1 a, type2 b, type3 *res*/);
bool __builtin_sadd_overflow(int a, int b, int* res);
bool __builtin_saddl_overflow(long int a, long int b, long int* res);
bool __builtin_saddll_overflow(long long int a, long long int b, long long int* res);
bool __builtin_uadd_overflow(unsigned int a, unsigned int b, unsigned int* res);
bool __builtin_uaddl_overflow(unsigned long int a, unsigned long int b, unsigned long int* res);
bool __builtin_uaddll_overflow(unsigned long long int a, unsigned long long int b, unsigned long long int* res);


double __builtin_huge_val(void);
float __builtin_huge_valf(void);
long double __builtin_huge_vall(void);


int __builtin_fpclassify(int, int, int, int, int, ...);
double __builtin_inf(void);

_Decimal32 __builtin_infd32(void);
_Decimal64 __builtin_infd64(void);
_Decimal128 __builtin_infd128(void);

float __builtin_inff(void);
long double __builtin_infl(void);

int __builtin_isinf_sign(...);
double __builtin_nan(const char* str);


_Decimal64 __builtin_nand64(const char* str);
_Decimal128 __builtin_nand128(const char* str);
float __builtin_nanf(const char* str);
long double __builtin_nanl(const char* str);



double __builtin_nans(const char* str);
_Decimal32 __builtin_nansd32(const char* str);
_Decimal64 __builtin_nansd64(const char* str);
_Decimal128 __builtin_nansd128(const char* str);
float __builtin_nansf(const char* str);
long double __builtin_nansl(const char* str);

int __builtin_issignaling(...);
double __builtin_powi(double, int);
float __builtin_powif(float, int);
long double __builtin_powil(long double, int);


int __builtin_abs(int x);
double __builtin_fabs(double x);
float __builtin_fabsf(float x);
long double __builtin_fabsl(long double x);

double __builtin_sin(double x);
float __builtin_sinf(float x);
long double __builtin_sinl(long double x);

double __builtin_cos(double x);
float __builtin_cosf(float x);
long double __builtin_cosl(long double x);

double __builtin_tan(double x);
float __builtin_tanf(float x);
long double __builtin_tanl(long double x);

double __builtin_sqrt(double x);
float __builtin_sqrtf(float x);
long double __builtin_sqrtl(long double x);

double __builtin_pow(double x, double y);
float __builtin_powf(float x, float y);
long double __builtin_powl(long double x, long double y);

double __builtin_exp(double x);
float __builtin_expf(float x);
long double __builtin_expl(long double x);

double __builtin_log(double x);
float __builtin_logf(float x);
long double __builtin_logl(long double x);

double __builtin_log10(double x);
float __builtin_log10f(float x);
long double __builtin_log10l(long double x);

double __builtin_floor(double x);
float __builtin_floorf(float x);
long double __builtin_floorl(long double x);

double __builtin_ceil(double x);
float __builtin_ceilf(float x);
long double __builtin_ceill(long double x);

int __builtin_ffs(int i);
int __builtin_ffsl(long i);
int __builtin_ffsll(long long i);

void* __builtin_memcpy(void* dest, const void* src, size_t n);
void* __builtin_memset(void* s, int c, size_t n);
int __builtin_memcmp(const void* s1, const void* s2, size_t n);

char* __builtin_strcpy(char* dest, const char* src);
char* __builtin_strncpy(char* dest, const char* src, size_t n);
int __builtin_strcmp(const char* s1, const char* s2);
size_t __builtin_strlen(const char* s);

_Noreturn void __builtin_abort(void);
_Noreturn void __builtin_exit(int status);
void __builtin_free(void* ptr);
void* __builtin_malloc(size_t size);
void* __builtin_realloc(void* ptr, size_t size);


int __builtin_signbit(double x);
int __builtin_signbitf(float x);
int __builtin_signbitl(long double x);

