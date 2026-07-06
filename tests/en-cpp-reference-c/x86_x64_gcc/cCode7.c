/* Cake x86_x64_gcc */

extern int long time(int long * __timer);
extern int printf(char * __format, ...);
extern char *ctime(int long * __timer);

int main(void)
{
    int long result;

    result = time(0);
    printf("%s", ctime(&result));
}


