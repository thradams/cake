#pragma cake diagnostic push
#pragma cake diagnostic ignored 11 /*style checks don't apply to builtins*/


typedef typeof(sizeof(1)) size_t;

/* clang builtin types cake has no keyword for. _Float16 (Apple math.h)
   aliased to float. The 128-bit ints only appear as declared fields in
   arm64 CPU/NEON state structs (never used in arithmetic here), so model
   them as a 16-byte aggregate rather than depending on _BitInt. */
typedef float _Float16;
typedef struct { unsigned long long __a, __b; } __uint128_t;
typedef struct { unsigned long long __a, __b; } __int128_t;


long __builtin_expect(long exp, long c);
int __builtin_constant_p(/*any expression*/...);

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

int __builtin_flt_rounds(void);




int __builtin_ffs(int x);
int __builtin_clz(unsigned int x);
int __builtin_ctz(unsigned int x);
int __builtin_clrsb(int x);
int __builtin_popcount(unsigned int x);
int __builtin_parity(unsigned int x);
int __builtin_ffsl(long);
int __builtin_clzl(unsigned long);
int __builtin_ctzl(unsigned long);
int __builtin_clrsbl(long);
int __builtin_popcountl(unsigned long);
int __builtin_parityl(unsigned long);
int __builtin_ffsll(long long);
int __builtin_clzll(unsigned long long);
int __builtin_ctzll(unsigned long long);
int __builtin_clrsbll(long long);
int __builtin_popcountll(unsigned long long);
int __builtin_parityll(unsigned long long);
int __builtin_ffsg(...);

int __builtin_clrsbg(...);
int __builtin_parityg(...);

unsigned int __builtin_stdc_bit_width(/*type arg*/);
unsigned int __builtin_stdc_count_ones(/*type arg*/);
unsigned int __builtin_stdc_count_zeros(/*type arg*/);
unsigned int __builtin_stdc_first_leading_one(/*type arg*/);
unsigned int __builtin_stdc_first_leading_zero(/*type arg*/);
unsigned int __builtin_stdc_first_trailing_one(/*type arg*/);
unsigned int __builtin_stdc_first_trailing_zero(/*type arg*/);
unsigned int __builtin_stdc_has_single_bit(/*type arg*/);
unsigned int __builtin_stdc_leading_ones(/*type arg*/);
unsigned int __builtin_stdc_leading_zeros(/*type arg*/);
unsigned int __builtin_stdc_trailing_ones(/*type arg*/);
unsigned int __builtin_stdc_trailing_zeros(/*type arg*/);


/*
  clang atomic builtins used by <stdatomic.h>.
  Type-generic: arguments are not checked and load, exchange and fetch_*
  return the type pointed by the first argument (see expressions.c).
*/
void __c11_atomic_init(/*_Atomic(T)* obj, T value*/);
void __c11_atomic_thread_fence(int order);
void __c11_atomic_signal_fence(int order);
_Bool __c11_atomic_is_lock_free(typeof(sizeof(1)) size);
void __c11_atomic_store(/*_Atomic(T)* obj, T value, int order*/);
int __c11_atomic_load(/*_Atomic(T)* obj, int order*/);
int __c11_atomic_exchange(/*_Atomic(T)* obj, T value, int order*/);
_Bool __c11_atomic_compare_exchange_strong(/*_Atomic(T)* obj, T* expected, T desired, int success, int failure*/);
_Bool __c11_atomic_compare_exchange_weak(/*_Atomic(T)* obj, T* expected, T desired, int success, int failure*/);
int __c11_atomic_fetch_add(/*_Atomic(T)* obj, T operand, int order*/);
int __c11_atomic_fetch_sub(/*_Atomic(T)* obj, T operand, int order*/);
int __c11_atomic_fetch_and(/*_Atomic(T)* obj, T operand, int order*/);
int __c11_atomic_fetch_or(/*_Atomic(T)* obj, T operand, int order*/);
int __c11_atomic_fetch_xor(/*_Atomic(T)* obj, T operand, int order*/);
int __c11_atomic_fetch_nand(/*_Atomic(T)* obj, T operand, int order*/);
int __c11_atomic_fetch_max(/*_Atomic(T)* obj, T operand, int order*/);
int __c11_atomic_fetch_min(/*_Atomic(T)* obj, T operand, int order*/);

/*
  https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
  Type-generic: arguments are not checked and the _n, fetch and op_fetch
  forms return the type pointed by the first argument (see expressions.c).
*/
int __atomic_load_n(/*T* ptr, int order*/);
void __atomic_load(/*T* ptr, T* ret, int order*/);
void __atomic_store_n(/*T* ptr, T val, int order*/);
void __atomic_store(/*T* ptr, T* val, int order*/);
int __atomic_exchange_n(/*T* ptr, T val, int order*/);
void __atomic_exchange(/*T* ptr, T* val, T* ret, int order*/);
_Bool __atomic_compare_exchange_n(/*T* ptr, T* expected, T desired, bool weak, int success, int failure*/);
_Bool __atomic_compare_exchange(/*T* ptr, T* expected, T* desired, bool weak, int success, int failure*/);
int __atomic_add_fetch(/*T* ptr, T val, int order*/);
int __atomic_sub_fetch(/*T* ptr, T val, int order*/);
int __atomic_and_fetch(/*T* ptr, T val, int order*/);
int __atomic_xor_fetch(/*T* ptr, T val, int order*/);
int __atomic_or_fetch(/*T* ptr, T val, int order*/);
int __atomic_nand_fetch(/*T* ptr, T val, int order*/);
int __atomic_max_fetch(/*T* ptr, T val, int order*/);
int __atomic_min_fetch(/*T* ptr, T val, int order*/);
int __atomic_fetch_add(/*T* ptr, T val, int order*/);
int __atomic_fetch_sub(/*T* ptr, T val, int order*/);
int __atomic_fetch_and(/*T* ptr, T val, int order*/);
int __atomic_fetch_xor(/*T* ptr, T val, int order*/);
int __atomic_fetch_or(/*T* ptr, T val, int order*/);
int __atomic_fetch_nand(/*T* ptr, T val, int order*/);
int __atomic_fetch_max(/*T* ptr, T val, int order*/);
int __atomic_fetch_min(/*T* ptr, T val, int order*/);
_Bool __atomic_test_and_set(void* ptr, int order);
void __atomic_clear(_Bool* ptr, int order);
void __atomic_thread_fence(int order);
void __atomic_signal_fence(int order);
_Bool __atomic_always_lock_free(typeof(sizeof(1)) size, void* ptr);
_Bool __atomic_is_lock_free(typeof(sizeof(1)) size, void* ptr);

#pragma cake diagnostic pop
