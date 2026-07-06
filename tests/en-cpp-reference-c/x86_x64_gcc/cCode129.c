/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    int a;
    int b;

    a = 1;
    b = 1;
    printf("original values: a == %d, b == %d\n", a, b);
    printf("result of postfix operators: a++ == %d, b-- == %d\n", a++, b--);
    printf("after postfix operators applied: a == %d, b == %d\n", a, b);
    printf("\n");
    a = 1;
    b = 1;
    printf("original values: a == %d, b == %d\n", a, b);
    printf("result of prefix operators: ++a == %d, --b == %d\n", ++a, --b);
    printf("after prefix operators applied: a == %d, b == %d\n", a, b);
}


