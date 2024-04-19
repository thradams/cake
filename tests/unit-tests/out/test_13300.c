void f(char* s, ...);
int main()
{
    int i;
    f("", i);
#pragma cake diagnostic check "-Wmaybe-uninitialized"
#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
    return 0;
}
