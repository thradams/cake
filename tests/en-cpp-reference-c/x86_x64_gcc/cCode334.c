/* Cake x86_x64_gcc */

extern char *strdup(char * __s);
extern int printf(char * __format, ...);
extern void free(void * __ptr);

int main(void)
{
    char * s1;
    char * s2;

    s1 = "Duplicate me!";
    s2 = strdup(s1);
    printf("s2 = \"%s\"\n", s2);
    free(s2);
}


