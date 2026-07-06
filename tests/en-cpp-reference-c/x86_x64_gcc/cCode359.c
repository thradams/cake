/* Cake x86_x64_gcc */

extern unsigned int long strlen(char * __s);
extern int mblen(char * __s, unsigned int long __n);
extern void perror(char * __s);

unsigned int long strlen_mb(char * ptr)
{
    unsigned int long result;
    char * end;

    result = 0;
    end = ptr + strlen(ptr);
    mblen(0, 0);
    while (ptr < end)
    {
        int next;

        next = mblen(ptr, end - ptr);
        if (next == -1)
        {
            perror("strlen_mb");
            break;
        }
        ptr += next;
        ++result;
    }
    return result;
}


extern int printf(char * __format, ...);

void dump_bytes(char * str)
{
    {
        char * end;

        end = str + strlen(str);
        for (; str != end; ++str)
        printf("%02X ", (unsigned char)str[0]);
    }
    printf("\n");
}


extern char *setlocale(int __category, char * __locale);

int main(void)
{
    char * str;

    setlocale(6, "en_US.utf8");
    str = "z\u00df\u6c34\U0001f34c";
    printf("The string \"%s\" consists of %zu characters, but %zu bytes: ", str, strlen_mb(str), strlen(str));
    dump_bytes(str);
}


