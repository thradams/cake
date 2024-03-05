__FILE__

struct X
{
    int i;
    void* _Owner p;
};
void f(struct X* p);
int main()
{
    struct X x = { 0 };
    f(x);
    static_state(x.p, "maybe-null");
}