/* Cake 0.14.05 x86_msvc */

struct X {
    char * p;
};

char *strdup(char * s);

int main()
{
    struct X x;

    x.p = strdup("a");
}
