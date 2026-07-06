/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern unsigned int short **__ctype_b_loc(void);

int main(void)
{
    unsigned char  b;
    int n;
    int x;
    char *a[2];

    b = 0;
    printf("!(2+2==4) = %s\n", b ? "true" : "false");
    n = ((*__ctype_b_loc())[97] & 8192);
    x = !!n;
    a[0] = "nonspace";
    a[1] = "space";
    printf("%s\n", a[x]);
}


