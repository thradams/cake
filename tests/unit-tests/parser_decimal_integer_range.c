#define IS_TYPE(T, E) _Generic(E, T: 1, default: 0)

#ifdef _WIN32
static_assert(IS_TYPE(int, 0));
static_assert(IS_TYPE(int, /*I32MAX*/ 2147483647));
static_assert(IS_TYPE(unsigned long, /*I32MAX + 1*/ 2147483648));
static_assert(IS_TYPE(unsigned long, /*U32MAX*/ 4294967295));
static_assert(IS_TYPE(long long, /*U32MAX + 1*/ 4294967296));
static_assert(IS_TYPE(long long, /*I64 MAX*/ 9223372036854775807));

//warning: integer literal is too large to be represented in a signed integer type, interpreting as unsigned 
[[cake::w50]]
static_assert(IS_TYPE(unsigned long long, /*I64 MAX + 1*/ 9223372036854775808));

//warning: integer literal is too large to be represented in a signed integer type, interpreting as unsigned 
[[cake::w50]]
static_assert(IS_TYPE(unsigned long long, /*U64 MAX */ 18446744073709551615));

/* no suffix */
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000));//
static_assert(IS_TYPE(unsigned long long, 0xFFFFFFFFFFFFFFFF));//

/* U */
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000U));//
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000u));//

/* L */
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000L));//
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000l));//

/* UL / LU */
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000UL));//
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000LU));//
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000ul));//
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000lu));//


#else

static_assert(IS_TYPE(int, 0));
static_assert(IS_TYPE(int, /*I32MAX*/ 2147483647));
static_assert(IS_TYPE(long, /*I32MAX + 1*/ 2147483648));
static_assert(IS_TYPE(long, /*U32MAX*/ 4294967295));
static_assert(IS_TYPE(long, /*U32MAX + 1*/ 4294967296));
static_assert(IS_TYPE(long, /*I64 MAX*/ 9223372036854775807));
//static_assert(IS_TYPE(__int128, /*I64 MAX + 1*/ 9223372036854775808));
//static_assert(IS_TYPE(__int128, /*U64 MAX */ 18446744073709551615));

/* no suffix */
static_assert(IS_TYPE(unsigned long, 0x8000000000000000));//
static_assert(IS_TYPE(unsigned long, 0xFFFFFFFFFFFFFFFF));//

/* U */
static_assert(IS_TYPE(unsigned long, 0x8000000000000000U));//
static_assert(IS_TYPE(unsigned long, 0x8000000000000000u));//

/* L */
static_assert(IS_TYPE(unsigned long, 0x8000000000000000L));//
static_assert(IS_TYPE(unsigned long, 0x8000000000000000l));//

/* UL / LU */
static_assert(IS_TYPE(unsigned long, 0x8000000000000000UL));//
static_assert(IS_TYPE(unsigned long, 0x8000000000000000LU));//
static_assert(IS_TYPE(unsigned long, 0x8000000000000000ul));//
static_assert(IS_TYPE(unsigned long, 0x8000000000000000lu));//

#endif



/* no suffix */
static_assert(IS_TYPE(unsigned int, 0x80000000));
static_assert(IS_TYPE(unsigned int, 0xFFFFFFFF));

/* U / u */
static_assert(IS_TYPE(unsigned int, 0x80000000U));
static_assert(IS_TYPE(unsigned int, 0x80000000u));

/* L / l */
//static_assert(IS_TYPE(long, 0x80000000L));
//static_assert(IS_TYPE(long, 0x80000000l));

/* UL / LU (order does not matter) */
static_assert(IS_TYPE(unsigned long, 0x80000000UL));
static_assert(IS_TYPE(unsigned long, 0x80000000LU));
static_assert(IS_TYPE(unsigned long, 0x80000000ul));
static_assert(IS_TYPE(unsigned long, 0x80000000lu));

/* LL */
static_assert(IS_TYPE(long long, 0x7FFFFFFFLL));
static_assert(IS_TYPE(long long, 0x7FFFFFFFll));

/* ULL / LLU */
static_assert(IS_TYPE(unsigned long long, 0x80000000ULL));
static_assert(IS_TYPE(unsigned long long, 0x80000000LLU));
static_assert(IS_TYPE(unsigned long long, 0x80000000ull));
static_assert(IS_TYPE(unsigned long long, 0x80000000llu));



/* LL */
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000LL));
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000ll));

/* ULL / LLU */
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000ULL));
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000LLU));
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000ull));
static_assert(IS_TYPE(unsigned long long, 0x8000000000000000llu));

