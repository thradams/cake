#pragma safety enable

void free(void* _Owner _Opt p);
char* _Owner strdup(const char* s);

struct Y {
    char* _Owner text;
};

struct X {
    
    struct Y* _Nonnull _Owner pY;
};

void f(struct X* _Opt pX)
{
    if (pX)
    {
        free(pX->pY->text);
        pX->pY->text = strdup("a");
    }
}
