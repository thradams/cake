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
extern unsigned int long mbrlen(char * __s, unsigned int long __n, struct __tag1 * __ps);
static struct __tag1  __c0_mb;
extern int mbsinit(struct __tag1 * __ps);
extern int printf(char * __format, ...);
extern unsigned int long strlen(char * __s);

int main(void)
{
    char * str;

    setlocale(6, "en_US.utf8");
    str = "\xe6\xb0\xb4";
    (void)mbrlen(&str[0], 1, &__c0_mb);
    if (!mbsinit(&__c0_mb))
    {
        printf("After processing the first 1 byte of %s,\nthe conversion state is not initial\n\n", str);
    }
    (void)mbrlen(&str[1], strlen(str), &__c0_mb);
    if (mbsinit(&__c0_mb))
    {
        printf("After processing the remaining 2 bytes of %s,\nthe conversion state is initial conversion state\n", str);
    }
}


