#pragma nullable enable
void free(void* _Owner _Opt p);
struct X
{
    int i;
    void* _Owner p;
};
void f(struct X* p);
int main()
{
    struct X x = { 0 };
    static_state(x.p, "null ");
    f(&x);
#pragma cake diagnostic check "-Wanalyzer-null-dereference"

    static_state(x.p, "not-null ");
    free(x.p);
}
