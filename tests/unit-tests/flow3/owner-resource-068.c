#pragma safety enable



char* _Owner strdup(const char* s);
void free(void* _Owner p);

struct X
{
    char* _Owner _Opt text;
};

void f(int a)
{
    struct X x  = {0}; // owner object (.text) not moved (root see line 16)
    x.text = strdup("a");
} //lint 29 owner object (.text) not moved (root see line 16)
