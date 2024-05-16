#pragma safety enable


struct X
{
    char* _Owner _Opt text;
};

void free(void* _Owner _Opt p);
void* _Owner _Opt calloc(int n, int sz);


struct X* _Owner make();
char* _Owner _Opt strdup(const char* s);

void f(int condition)
{
    struct X* _Owner _Opt p = calloc(1, sizeof * p);
    try
    {
        if (p == nullptr)
            throw;


        if (condition)
            throw;
        p->text = strdup("a");
    }
    catch
    {
    }
}
