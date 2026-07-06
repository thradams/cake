/* Cake x86_x64_gcc */

extern int iswctype(unsigned int __wc, unsigned int long __desc);
extern unsigned int long wctype(char * __property);

char *classify(int wc, char * cat)
{
    return iswctype(wc, wctype(cat)) ? "true" : "false";
}


extern char *setlocale(int __category, char * __locale);
extern int puts(char * __s);
extern int printf(char * __format, ...);

int main(void)
{
    char *cats[5];

    setlocale(6, "ja_JP.UTF-8");
    puts("The character \u6c34 is...");
    cats[0] = "digit";
    cats[1] = "alpha";
    cats[2] = "space";
    cats[3] = "cntrl";
    cats[4] = "jkanji";
    {
        int n;

        n = 0;
        for (; n < 5; ++n)
        printf("%s?\t%s\n", cats[n], classify(27700, cats[n]));
    }
}


