/* Cake x86_x64_gcc */

extern int wctob(unsigned int __c);
extern int printf(char * __format, ...);

void try_narrowing(int c)
{
    int cn;

    cn = wctob(c);
    if (cn != -1)
    {
        printf("%#x narrowed to %#x\n", c, cn);
    }
    else
    {
        printf("%#x could not be narrowed\n", c);
    }
}


extern char *setlocale(int __category, char * __locale);
extern int puts(char * __s);

int main(void)
{
    char * utf_locale_present;
    char * tis_locale_present;

    utf_locale_present = setlocale(6, "th_TH.utf8");
    ;
    puts("In Thai UTF-8 locale:");
    try_narrowing(97);
    try_narrowing(3675);
    tis_locale_present = setlocale(6, "th_TH.tis620");
    ;
    puts("In Thai TIS-620 locale:");
    try_narrowing(97);
    try_narrowing(3675);
}


