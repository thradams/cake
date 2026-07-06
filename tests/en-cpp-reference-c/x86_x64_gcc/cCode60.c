/* Cake x86_x64_gcc */

extern char *setlocale(int __category, char * __locale);
extern unsigned int putwchar(int __wc);
extern int puts(char * __s);

int main()
{
    int data[3];

    setlocale(6, "en_US.utf8");
    data[0] = 8704;
    data[1] = 8704;
    data[2] = 10;
    {
        unsigned int long t;

        t = 0;
        for (; t != 3L; ++t)
        {
            if (putwchar(data[t]) == 4294967295)
            {
                puts("I/O error in putwchar");
                return 1;
            }
        }
    }
    return 0;
}


