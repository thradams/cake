/* flow.md A15.4, A15.5: [[nodiscard]] may be added by the pointer; [[noreturn]] must match */
#pragma safety enable

[[nodiscard]] int h_nd(void);
int h(void);
[[noreturn]] void e(void);
void n(void);

void f()
{
    [[nodiscard]] int (*pn)(void) = h;
    int (*pn2)(void) = h_nd; //lint 54
    [[noreturn]] void (*pe)(void) = e;
    [[noreturn]] void (*pe2)(void) = n; //lint 54
    void (*pe3)(void) = e; //lint 54
}
