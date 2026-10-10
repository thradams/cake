/* N3884: break and continue cannot leave a local function */

void f(void)
{
    for (;;)
    {
        static void r1(void) { break; } //lint 760
        static void r2(void) { continue; } //lint 770
        static void r3(int n) { for (;;) { if (n) continue; break; } }
        break;
    }
}
