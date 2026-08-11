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
    struct X x = makex(); // owner object (x.text) not moved (root see line 17)
    clear(&x); // passing a possible uninitialized object  '(*&x).text' (see line 17)
    _Assert(x.text == 0);
}
