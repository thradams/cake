/* Cake 0.12.26 x86_msvc */
struct X {
    char * owner_variable;
};


struct X  global;
char *f()
{
    return global.owner_variable;
}


