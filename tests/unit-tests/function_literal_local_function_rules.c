/* N3885: a function literal is a local function definition, so the N3884 rules apply */

struct S { int a[3]; };

void f(int n)
{
    constexpr struct S s = {{1, 2, 3}};
    constexpr int K = 5;

    for (;;)
    {
        (void)(static void (void)) { break; }; //lint 760
        (void)(static void (void)) { continue; }; //lint 770
        break;
    }

    (void)(static const int* (void)) { return &K; }; //lint 1220
    (void)(static int (void)) { return *(s.a + 1); }; //lint 950
    (void)(static const int* (void)) { return &s.a[1]; }; //lint 1220

    (void)(static int (int i)) { return K + s.a[1] + s.a[i]; };
    (void)(static void (int m)) { for (;;) { if (m) continue; break; } };
    (void)(static void (void)) { goto L; L:; };

    switch (n)
    {
    case 1:
        (void)(static void (void)) { case 2: ; }; //lint 750
        (void)(static void (void)) { default: ; }; //lint 750
        (void)(static void (int m)) { switch (m) { case 1: break; default: break; } };
        break;
    }
}
