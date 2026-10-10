/* N3884: a local function used in a non-discarded expression must be defined in the same scope */

void f(void)
{
    static int u(void);
    u();

    static int d(void);
    d();
    static int d(void) { return 0; }

    static int s(void);
    (void)sizeof s();
    typeof(s()) x = 0;
    (void)x;
} //lint 1890 'u' used but not defined
