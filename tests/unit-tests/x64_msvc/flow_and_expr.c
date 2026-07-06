/* Cake x64_msvc */

char *f();
int strcmp(char * s1, char * s2);

void g()
{
    char * s1;

    s1 = f();
    if (s1 && strcmp(s1, "a") == 0)
    {
    }
    else
    {
    }
}
