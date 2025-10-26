/* Cake 0.12.26 x86_msvc */
struct X {
    char * p;
};


char *strdup(char * s);

int main()
{
    struct X  x;

    x.p = strdup("a");
}


