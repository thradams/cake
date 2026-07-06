/* Cake x86_x64_gcc */

extern char *strdup(char * __s);
extern void free(void * __ptr);

int main(void)
{
    char * s1;
    char * s2;

    s1 = "String";
    s2 = strdup(s1);
    ;
    free(s2);
}


