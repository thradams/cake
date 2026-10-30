#pragma safety enable

struct X { int* p; int* _Nonnull q; };

/* *pp is shared with the caller: reading it is the worst case, it may be null (flow.md 2.2) */
void by_parameter(struct X** pp)
{
    *(*pp)->p = 1; //lint 33 33
    *(*pp)->q = 1; //lint 33
}

struct X* _Nonnull* _Nonnull get(void);
void by_result(void)
{
    struct X* _Nonnull* pp = get();
    *(*pp)->p = 1; //lint 33
    *(*pp)->q = 1;
}
