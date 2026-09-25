/* valid constants (the invalid ones are checked by parse_number_test in token.c) */
static_assert(0x123e == 4670);
static_assert(0xFFULL == 255);
static_assert(_Generic(0xFFULL, unsigned long long : 1, default : 0));
static_assert(0x1p+1 == 2.0);
static_assert(0x1.8p3 == 12.0);
static_assert(_Generic(0x1.8p3, double : 1, default : 0));
static_assert(052 == 42);
static_assert(100 == 100);
static_assert(1e+1 == 10.0);
static_assert(_Generic(1.5e-3f, float : 1, default : 0));
static_assert(.5e2 == 50.0);

/* digit separators */
static_assert(1'00'00 == 10000);
static_assert(0b1010'10 == 42);
static_assert(0xAB'CD == 0xABCD);

/* microsoft suffixes i8 i16 i32 i64 normalized to the standard ones */
static_assert(_Generic(1i8, int : 1, default : 0));
static_assert(_Generic(1i16, int : 1, default : 0));
static_assert(_Generic(1i32, int : 1, default : 0));
static_assert(_Generic(1i64, long long : 1, default : 0));
static_assert(_Generic(1I64, long long : 1, default : 0));
static_assert(_Generic(1ui64, unsigned long long : 1, default : 0));
static_assert(_Generic(1Ui64, unsigned long long : 1, default : 0));
static_assert(_Generic(1uI64, unsigned long long : 1, default : 0));
static_assert(_Generic(1UI64, unsigned long long : 1, default : 0));
static_assert(_Generic(1ui8, unsigned int : 1, default : 0));
static_assert(_Generic(1ui16, unsigned int : 1, default : 0));
static_assert(_Generic(1ui32, unsigned int : 1, default : 0));
static_assert(_Generic(0x1ui64, unsigned long long : 1, default : 0));
static_assert(_Generic(0b1ui64, unsigned long long : 1, default : 0));
static_assert(_Generic(01ui64, unsigned long long : 1, default : 0));
static_assert(_Generic(0x8a44000000000040Ui64, unsigned long long : 1, default : 0));
static_assert(0x8a44000000000040Ui64 == 0x8a44000000000040ULL);
