
int main(void)
{
    int i32_max = 2147483647;
    long long u32_max = 4294967295;
    long long i64_max = 9223372036854775807;
    unsigned long long u64_max = 18446744073709551615;

    static_assert(_Generic(2147483647, int: 1, default: 0));
    static_assert(_Generic(2147483648, unsigned long: 1, default: 0));

    static_assert(_Generic(4294967295, unsigned long : 1, default: 0));
    static_assert(_Generic(9223372036854775806 + 1, long long : 1, default: 0));
    //static_assert(_Generic(9223372036854775807, unsigned long long : 1, default: 0));
    static_assert(_Generic(18446744073709551615, unsigned long long : 1, default: 0));
}