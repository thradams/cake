#pragma safety enable

struct X { int* _Opt p; int* q; };

void by_parameter(struct X** pp)
{
    *(*pp)->p = 1; //lint 33
    *(*pp)->q = 1;
}

struct X** get(void);
void by_result(void)
{
    struct X** pp = get();
    *(*pp)->p = 1; //lint 33
    *(*pp)->q = 1;
}
