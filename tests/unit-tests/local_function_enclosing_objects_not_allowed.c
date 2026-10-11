/* A local function cannot use automatic objects of the enclosing function, even constexpr */

struct X { int a[2];};

void f(struct X x)
{}

int main()
{
    constexpr int i = 1;
    constexpr struct X x = {1,2};
    static struct X local()
    {
        int j = i; //lint 1870
        f(x); //lint 1870
        f(x); //lint 1870
        return x; //lint 1870
    }
    
    local();
}
