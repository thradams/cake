#pragma safety enable

int g(char* s, int n, const char* _Opt argv[]);

void f(char* s)
{
    const char* argv[100] = { 0 };
    g(s, 10, argv); //lint 54 argv is not _Opt but starts null (flow.md A15.2)
}
