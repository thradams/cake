void free(void* _Owner p);
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
    free(x.p);
}