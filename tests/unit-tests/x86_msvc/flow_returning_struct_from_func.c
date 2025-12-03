/* Cake x86_msvc */
struct X {
    char * name;
};


struct X f();

struct X f2()
{
    return f();
}


