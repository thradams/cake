


/*function sample*/
extern int func(void);
auto p_func = func;
const auto pc_func = func;

/*using auto inside a macro*/
#define SWAP(a, b)   do {    auto temp = a; a = b; b = temp;   } while (0)

#pragma expand SWAP

void f()
{
    int a = 1;
    int b = 2;
    SWAP(a, b);
}

auto literal_string = "hello auto";

struct { int i; } x;
auto x2 = x;

auto bb = true;
auto pnull = nullptr;

/*arrays*/
int a5[5];
auto a = a5; /*lvalue*/

auto pa = &a5;

const auto cpa = &a5;

void f2(int a[2])
{
    auto p = a;
}


int main()
{
    double const x = 78.9;
    double y = 78.9;
    auto q = x;
    auto const p = &x;
    auto const r = &y;
}


