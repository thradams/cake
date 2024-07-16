#pragma safety enable

struct X {
    int i;
};

void f(struct X* /*_Opt*/ p)
{
    int i = p ? p->i : 0; //no warning
    
    int i3 = p->i;
    #pragma cake diagnostic check "-Wanalyzer-null-dereference"

    int i2 = p ? 0 : p->i; //warning
#pragma cake diagnostic check "-Wanalyzer-null-dereference"
}
