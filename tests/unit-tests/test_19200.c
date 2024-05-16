#pragma safety enable

void free(void* _Owner _Opt p);
char* _Owner _Opt strdup(const char* s);


struct X {

    char* _Owner _Opt text;
};
#define NULL ((void*)0)

struct X makex();
void clear(struct X* p);
void f(struct X* _Opt pX)
{
    struct X x = makex();
    clear(&x);
    assert(x.text == 0);
}
