/* Cake x64_msvc */

struct X {
    char * owner_variable;
};

char *f(struct X * parameter)
{
    return parameter->owner_variable;
}
