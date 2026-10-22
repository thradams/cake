/*
 * LLONG_MIN / -1 and LLONG_MIN % -1. The quotient overflows, and the host
 * division traps on x64 (0xC0000095), so the flow analysis, the
 * preprocessor and the constant folding must not do it natively.
 */
#pragma safety enable

long long f = (-9223372036854775807LL - 1) / -1; //lint 37 integer overflow
long long g = (-9223372036854775807LL - 1) % -1;

#if (-9223372036854775807LL - 1) / -1 != (-9223372036854775807LL - 1)
#error the quotient wraps to LLONG_MIN
#endif

#if (-9223372036854775807LL - 1) % -1 != 0
#error the remainder is 0
#endif

int main(void)
{
    long long a = -9223372036854775807LL - 1;
    long long b = -1;
    long long q = a / b;
    long long r = a % b;
    a /= b;
    a %= b;
    return (int)(q + r + a);
}
