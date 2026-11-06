/* a op= b with b known only by the flow, and every operator folds per value */
#pragma safety enable

void f(int c)
{
    int k = 3;
    int a = c ? 12 : 10;
    a += k;
    compile_assert(a == 15 || a == 13);
    int m = 6;
    m &= 3;
    compile_assert(m == 2);
    m |= 1;
    compile_assert(m == 3);
    m ^= 1;
    compile_assert(m == 2);
    m <<= 100; /* out of range: unknown, not computed by the host */
    compile_assert(m == 2); //lint 67
}
