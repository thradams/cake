/* Cake 0.12.26 x86_msvc */

char *strdup(char * s);
void exit(int exit_code);
void free(void * p);

void f()
{
    char * s;

    s = strdup("a");
    if (s == 0)
    {
        exit(1);
    }
    free(s);
}


