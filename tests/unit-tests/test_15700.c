#pragma nullable enable
void free(void* _Owner p);
struct X
{
    int i;
    void* _Owner p;
};
void f(const struct X* p);
int main()
{
    struct X x = { 0 };
    f(&x);
#pragma cake diagnostic check "-Wanalyzer-null-dereference"

    static_state(x.p, "null ");
}