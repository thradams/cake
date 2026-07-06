/* Cake x86_x64_gcc */

extern int wcsncmp(int * __s1, int * __s2, unsigned int long __n);
extern int printf(char * __format, ...);

void demo(int * lhs, int * rhs, int sz)
{
    int rc;

    rc = wcsncmp(lhs, rhs, sz);
    if (rc == 0)
    {
        printf("First %d characters of [%ls] equal [%ls]\n", sz, lhs, rhs);
    }
    else
    {
        if (rc < 0)
        {
            printf("First %d characters of [%ls] precede [%ls]\n", sz, lhs, rhs);
        }
        else
        {
            if (rc > 0)
            {
                printf("First %d characters of [%ls] follow [%ls]\n", sz, lhs, rhs);
            }
        }
    }
}


extern char *setlocale(int __category, char * __locale);

int main(void)
{
    int * str1;
    int * str2;

    str1 = L"\xec\x95\x88\xeb\x85\x95\xed\x95\x98\xec\x84\xb8\xec\x9a\x94";
    str2 = L"\xec\x95\x88\xeb\x85\x95\xed\x9e\x88 \xea\xb0\x80\xec\x8b\xad\xec\x8b\x9c\xec\x98\xa4";
    setlocale(6, "en_US.utf8");
    demo(str1, str2, 5);
    demo(str2, str1, 8);
    demo(str1, str2, 2);
}


