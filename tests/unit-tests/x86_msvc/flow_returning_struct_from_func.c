/* Cake 0.12.26 x86_msvc */
struct X {
    char * name;
};


struct X f();

struct X f2()
{
    return f();
}


