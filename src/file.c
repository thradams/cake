union __tag5 {
    signed long long host_long_long;
    unsigned long long host_u_long_long;
    long double host_long_double;
};
static_assert(sizeof(union __tag5) == 16);