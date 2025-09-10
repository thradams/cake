#pragma safety enable


char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct X {
    char* _Owner _Opt name;
};

struct X make()
{
    struct X x = {0};
    x.name = strdup("text");
    free(x.name);
    //warning: returning an uninitialized 'x.name' object 
    [[cake::w30]]
    return x;
}

