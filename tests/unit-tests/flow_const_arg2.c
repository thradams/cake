#pragma safety enable



void free(void* _Owner p);
struct X
{
    int i;
    void* _Owner _Opt p;
};
void f(const struct X* _Opt p);
int main()
{
    struct X x = { 0 };
    f(&x);
    static_state(x.p, "null");
}
