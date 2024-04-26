#pragma safety enable


void free(void* _Owner _Opt p);
struct X
{
    int i;
    void* _Owner _Opt p;
};
void f(struct X* _Opt p);
int main()
{
    struct X x = { 0 };
    static_state(x.p, "null");
    f(&x);
    static_state(x.p, "null | not-null");
    free(x.p);
}
