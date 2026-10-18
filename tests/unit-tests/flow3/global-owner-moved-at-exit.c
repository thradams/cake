#pragma safety enable

void free(void* _Owner _Opt p);
char* _Owner _Opt strdup(const char* s);

struct X { char* _Owner _Opt text; };
struct X gx;
char* _Owner _Opt gs;

void f1(void)
{
    free(gx.text);
} //lint 72

void f2(void)
{
    free(gx.text);
    gx.text = strdup("a");
}

void f3(void)
{
    free(gs);
} //lint 72

void f4(void)
{
    free(gs);
    gs = 0;
}

void f5(int c)
{
    if (c)
    {
        free(gx.text);
    }
} //lint 72
