/* N3884: block scope function declarations can only use extern or static */

void f(void)
{
    auto void a(void); //lint 1890
    register void r(void); //lint 1890
    static void s(void) { }
    extern void e(void);
    s();
}
