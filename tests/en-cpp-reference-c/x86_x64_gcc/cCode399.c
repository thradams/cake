/* Cake x86_x64_gcc */

extern int *wcspbrk(int * __wcs, int * __accept);
extern unsigned int long wcsspn(int * __wcs, int * __accept);
extern int wprintf(int * __format, ...);

int main(void)
{
    int * str;
    int * sep;
    unsigned int cnt;

    str = L"Hello world, friend of mine!";
    sep = L" ,!";
    cnt = 0;
    do
    {
        str = wcspbrk(str, sep);
        if (str)
        {
            str += wcsspn(str, sep);
        }
        ++cnt;
    }
    while (str && *str);
    wprintf(L"There are %u words.\n", cnt);
}


