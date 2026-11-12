#pragma safety enable

/*
    A comparison of two known values is the one of the language: the operands
    are converted to the common type first, so -1 and 4294967295u are equal as
    unsigned int, and -1 is greater than 1u.
*/

void mixed_signedness_equality(void)
{
    int a = -1;
    unsigned int b = 4294967295u;
    compile_assert(a == b);
    compile_assert(!(a != b));
}

void mixed_signedness_order(void)
{
    int a = -1;
    unsigned int b = 1;
    compile_assert(a > b);
    compile_assert(a >= b);
    compile_assert(!(a < b));
    compile_assert(!(a <= b));
}

void same_signedness_keeps_working(void)
{
    int a = -1;
    int b = 1;
    compile_assert(a < b);
    compile_assert(a <= b);
    compile_assert(!(a > b));
    compile_assert(a != b);
    compile_assert(!(a == b));
}

void narrow_types_are_promoted(void)
{
    unsigned char c = 200;
    signed char s = -56;
    compile_assert(c != s);
    compile_assert(c > s);
}

void unsigned_wrap_compares_as_unsigned(void)
{
    unsigned int u = 0;
    compile_assert(u - 1 > 0);
}
