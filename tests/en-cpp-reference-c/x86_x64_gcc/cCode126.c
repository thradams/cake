/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    int x;
    int hundred;
    int ten;
    int fifty;

    x = 10;
    hundred = 100;
    ten = 10;
    fifty = 50;
    printf("%d %d %d %d\n", x, hundred, ten, fifty);
    hundred *= x;
    ten /= x;
    fifty %= x;
    printf("%d %d %d %d\n", x, hundred, ten, fifty);
    return 0;
}


