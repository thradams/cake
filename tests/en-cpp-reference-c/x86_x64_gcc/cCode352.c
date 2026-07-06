/* Cake x86_x64_gcc */

extern int toupper(int __c);
extern int printf(char * __format, ...);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    unsigned char c;

    {
        unsigned char l;

        l = 0;
        for (; l < 255; l++)
        {
            unsigned char u;

            u = toupper(l);
            if (u != l)
            {
                printf("%c%c ", l, u);
            }
        }
    }
    printf("\n\n");
    c = 184;
    setlocale(6, "en_US.iso88591");
    printf("in iso8859-1, toupper('0x%x') gives 0x%x\n", c, toupper(c));
    setlocale(6, "en_US.iso885915");
    printf("in iso8859-15, toupper('0x%x') gives 0x%x\n", c, toupper(c));
}


