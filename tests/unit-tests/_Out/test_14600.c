struct X { int i; void* /*_Opt*/ p; };
void f(struct X* p) {}

int main()
{
    const struct X x = {0};
    f(&x);
#pragma cake diagnostic check "-Wdiscarded-qualifiers"
}
