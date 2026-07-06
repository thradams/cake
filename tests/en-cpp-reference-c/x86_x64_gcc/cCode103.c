/* Cake x86_x64_gcc */

extern int rand(void);
extern int puts(char * __s);
extern int printf(char * __format, ...);

int main(void)
{
    int array[8];
    int n;
    int num;
    int i;
    char s[10];

    n = 0;
    do
    array[n++] = rand() % 2;
    while (n < 8);
    puts("Array filled!");
    n = 0;
    do
    {
        printf("%d ", array[n]);
        ++n;
    }
    while (n < 8);
    printf("\n");
    num = 1234;
    i = 0;
    do
    s[i++] = num % 10 + 48;
    while ((num /= 10) > 0);
    s[i] = 0;
    puts(s);
}


