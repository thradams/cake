/* Cake x86_x64_gcc */

extern int tolower(int __c);
extern int printf(char * __format, ...);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    unsigned char c;

    {
        unsigned char u;

        u = 0;
        for (; u < 255; u++)
        {
            unsigned char l;

            l = tolower(u);
            if (l != u)
            {
                printf("%c%c ", u, l);
            }
        }
    }
    printf("\n\n");
    c = 180;
    setlocale(6, "en_US.iso88591");
    printf("in iso8859-1, tolower('0x%x') gives 0x%x\n", c, tolower(c));
    setlocale(6, "en_US.iso885915");
    printf("in iso8859-15, tolower('0x%x') gives 0x%x\n", c, tolower(c));
}


