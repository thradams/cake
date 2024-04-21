#pragma nullable enable
#pragma ownership enable

void free(void* /*_Owner*/ _Opt p);
struct X
{
    int i;
    void* /*_Owner*/ p;
};
void f(struct X* p);
int main()
{
    struct X x = { 0 };
    
    f(&x);
#pragma cake diagnostic check "-Wanalyzer-null-dereference"

    
    free(x.p);
}
