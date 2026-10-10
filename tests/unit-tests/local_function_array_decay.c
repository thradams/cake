/* N3884: an array of the enclosing function cannot decay to a pointer in a local function */

struct S { int a[3]; };

int f(void)
{
    constexpr struct S s = {{1, 2, 3}};
    static const int* r1(void) { return s.a; } //lint 950
    static int r2(void) { return *(s.a + 1); } //lint 950
    static const int* r3(void) { return &s.a[1]; } //lint 1220
    static int r4(int i) { return s.a[1] + s.a[i] + (int)sizeof(s.a + 1); }
    static struct S r5(void) { return s; }

    constexpr int A[3] = {1, 2, 3};
    static int r6(void) { return *(A + 1); } //lint 950
    static const int* r7(void) { return &A[1]; } //lint 1220

    const int* p = s.a + 1;
    (void)r1;
    (void)r2;
    (void)r3;
    (void)r6;
    (void)r7;
    return r4(0) + r5().a[0] + *p;
}
