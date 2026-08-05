#pragma safety enable

/*
   Regression test for flow3_cast_integer_value.

   The function used the result of type_get_sizeof (bytes) directly as a
   bit width, so every truncating integer cast folded to a wrong value
   (e.g. (signed char)300 was masked with 0x1 instead of 0xFF).
*/

void cast_to_signed_char()
{
    int big = 300;
    signed char c = (signed char)big;
    compile_assert(c == 44); /* 300 & 0xFF = 44 */
}

void cast_to_signed_char_negative()
{
    int v = 200;
    signed char sc = (signed char)v;
    compile_assert(sc == -56); /* 200 - 256 */
}

void cast_to_unsigned_char()
{
    int v = -1;
    unsigned char uc = (unsigned char)v;
    compile_assert(uc == 255);
}

void cast_to_short()
{
    int v = 70000;
    short s = (short)v;
    compile_assert(s == 4464); /* 70000 & 0xFFFF */
}

void cast_no_truncation()
{
    int v = 42;
    long long ll = (long long)v;
    compile_assert(ll == 42);
}
