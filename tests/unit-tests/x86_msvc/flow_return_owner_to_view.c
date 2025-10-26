/* Cake 0.12.26 x86_msvc */
struct X {
    char * owner_variable;
};


char *f(struct X * parameter)
{
    return parameter->owner_variable;
}


