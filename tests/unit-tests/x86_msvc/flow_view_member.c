/* Cake 0.12.26 x86_msvc */
struct X {
    char * text;
};

struct Y {
    struct X  x;
};


struct Y f();

int main()
{
    struct Y  y;

    y = f();
}


