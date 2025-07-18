
int _Avx2WmemEnabledWeakValue = 0;
char *__cdecl setlocale(int _Category, char * _Locale);
unsigned short __cdecl putwchar(unsigned short _Character);
int __cdecl puts(char * _Buffer);

int main()
{
    unsigned short data[3];

    setlocale(0, "en_US.utf8");
    data[0] = 8704;
    data[1] = 8704;
    data[2] = 10;
    {
        unsigned int t;
        t = 0;
        for (; t != 3U; ++t)
        {
            if (putwchar(data[t]) == 65535)
            {
                puts("I/O error in putwchar");
                return 1;
            }
        }
    }
    return 0;
}

