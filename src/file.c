#pragma safety enable

struct X { char* s; };
struct X f()
{
    struct X x = {};
    return x;
}