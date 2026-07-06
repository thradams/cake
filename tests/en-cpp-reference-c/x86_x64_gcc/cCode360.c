/* Cake x86_x64_gcc */
union __tag2 {
    unsigned int __wch;
    char __wchb[4];
};

struct __tag1 {
    int __count;
    union __tag2  __value;
};


extern char *setlocale(int __category, char * __locale);
extern unsigned int long strlen(char * __s);
extern void *memset(void * __s, int __c, unsigned int long __n);
extern unsigned int long mbrlen(char * __s, unsigned int long __n, struct __tag1 * __ps);
extern int printf(char * __format, ...);

int main(void)
{
    char * str;
    unsigned int long sz;
    struct __tag1  mb;
    int len1;
    int len2;

    setlocale(6, "en_US.utf8");
    str = "\xe6\xb0\xb4";
    sz = strlen(str);
    memset(&mb, 0, 8L);
    len1 = mbrlen(str, 1, &mb);
    if (len1 == -2)
    {
        printf("The first 1 byte of %s is an incomplete multibyte char (mbrlen returns -2)\n", str);
    }
    len2 = mbrlen(str + 1, sz - 1, &mb);
    printf("The remaining %zu  bytes of %s hold %d bytes of the multibyte character\n", sz - 1, str, len2);
    printf("Attempting to call mbrlen() in the middle of %s while in initial shift state returns %zd\n", str, mbrlen(str + 1, sz - 1, &mb));
}


