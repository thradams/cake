/* -1 does not fit unsigned long long; the old check looked only at the low 64 bits */
enum E : unsigned long long {
    A = -1
}; //lint 860 enumerator value outside of underlying type range
