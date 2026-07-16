/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

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
