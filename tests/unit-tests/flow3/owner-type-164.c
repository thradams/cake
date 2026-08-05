#pragma safety enable
 
struct X {
    char * _Owner text;
};

struct Y {
    _View struct X x;
};

struct Y f();
int main()
{
    struct Y y = f(); // ok: y.x is _View, so its _Owner members are views (not "owner not moved")
}
