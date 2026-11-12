#pragma safety enable

/*
    The arithmetic of known values is the one of the language: operands are
    promoted, an unsigned result wraps, and an operation the language leaves
    undefined has no value.
*/

void unsigned_subtraction_wraps(void)
{
    unsigned int u = 0;
    unsigned int v = u - 1;
    compile_assert(v == 4294967295u);
    compile_assert(v > 0);
}

void unsigned_addition_wraps(void)
{
    unsigned char c = 255;
    unsigned char d = c + 1;
    compile_assert(d == 0);
}

void unsigned_increment_wraps(void)
{
    unsigned int u = 4294967295u;
    u++;
    compile_assert(u == 0);
}

void unsigned_multiplication_wraps(void)
{
    unsigned int a = 65536;
    unsigned int b = a * a;
    compile_assert(b == 0);
}

void signed_arithmetic_is_kept(void)
{
    int a = 7;
    int b = 3;
    compile_assert(a + b == 10);
    compile_assert(a - b == 4);
    compile_assert(a * b == 21);
    compile_assert(a / b == 2);
    compile_assert(a % b == 1);
    compile_assert((a << 2) == 28);
    compile_assert((a >> 1) == 3);
    compile_assert((a & b) == 3);
    compile_assert((a | b) == 7);
    compile_assert((a ^ b) == 4);
}

void promotion_to_int(void)
{
    char c = 100;
    char d = 100;
    int sum = c + d;
    compile_assert(sum == 200);
}

void mixed_signedness(void)
{
    int a = -1;
    unsigned int b = 1;
    compile_assert(a + b == 0);
    compile_assert((long long)(a + b) == 0);
}

void assignment_converts_to_the_destination_type(void)
{
    int big = 300;
    unsigned char c = big;
    compile_assert(c == 44);

    int negative = -1;
    unsigned int u = negative;
    compile_assert(u == 4294967295u);

    long long wide = 4294967297;
    int narrow = wide;
    compile_assert(narrow == 1);

    int five = 5;
    _Bool b = five;
    compile_assert(b == 1);

    int multiple_of_256 = 256;
    _Bool c2 = multiple_of_256;
    compile_assert(c2 == 1);

    int zero = 0;
    _Bool z = zero;
    compile_assert(z == 0);
}

void bitwise_with_unknown_operand(int x)
{
    int a = 6;
    int r = x & a;
    compile_assert(r == 0); //lint 67
    compile_assert((x | 0) == 0); //lint 67
}

void bitwise_of_unsigned(void)
{
    unsigned int a = 4294967295u;
    unsigned int b = 255;
    compile_assert((a & b) == 255);
    compile_assert((a ^ b) == 4294967040u);
}
