#pragma safety enable

_Noreturn void die(int);
[[maybe_unused]] _Noreturn void die2(int);
int* get(void);

void keyword_noreturn(void)
{
    int* p = get();
    if (p == 0) die(1);
    *p = 5; /* clean: die does not return */
}

void keyword_noreturn_after_attribute(void)
{
    int* p = get();
    if (p == 0) die2(1);
    *p = 5;
}

void die3(int);
[[noreturn]] void die3(int);

void attribute_on_redeclaration(void)
{
    int* p = get();
    if (p == 0) die3(1);
    *p = 5;
}
