/* ULLONG_MAX does not fit long long */
enum E : long long {
    A = 18446744073709551615ULL
}; //lint 860 enumerator value outside of underlying type range
