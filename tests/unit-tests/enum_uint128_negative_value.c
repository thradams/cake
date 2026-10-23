#ifdef __SIZEOF_INT128__
#define I128_MAX ((__int128)(((unsigned __int128)1 << 127) - 1))
#define I128_MIN (-I128_MAX - 1)
#define U128_MAX (~(unsigned __int128)0)

enum E : unsigned __int128 {
    A = -1
}; //lint 860 enumerator value outside of underlying type range

#endif
