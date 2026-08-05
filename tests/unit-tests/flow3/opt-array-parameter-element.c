#pragma safety enable

int g(char* s, int n, const char* _Opt argv[]);

void f(char* s)
{
    const char* argv[100] = { 0 };
    g(s, 10, argv);
}
