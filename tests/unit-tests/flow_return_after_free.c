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
    
    return x; //lint 30 warning: returning an uninitialized 'x.name' object 
}

