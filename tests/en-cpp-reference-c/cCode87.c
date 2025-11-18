//en.cppreference.com/w/c/language/attributes/deprecated.html
#include <stdio.h>

[[deprecated]]
void TriassicPeriod(void)
{
    puts("Triassic Period: [251.9 - 208.5] million years ago.");
}

[[deprecated("Use NeogenePeriod() instead.")]]
void JurassicPeriod(void)
{
    puts("Jurassic Period: [201.3 - 152.1] million years ago.");
}

[[deprecated("Use calcSomethingDifferently(int).")]]
int calcSomething(int x)
{
    return x * 2;
}

int main(void)
{
    [[cake::w3]]
    TriassicPeriod();

    [[cake::w3]]
    JurassicPeriod();
}