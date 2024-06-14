#pragma safety enable
 
struct X {
    char * /*_Owner*/ text;
};

struct Y {
    _View struct X x;
};

struct Y f();
int main()
{
    struct Y y = f();
}



