/* Cake 0.13.29 x86_msvc */

static void memcpy2(void * dest, const void * src, unsigned int n);

char *__cdecl strcat(char * _Destination, char * _Source);
char *__cdecl strncat(char * _Destination, char * _Source, unsigned int _Count);
int __cdecl puts(char * _Buffer);

int main(void)
{
    char str[50];
    char str2[50];

    memcpy2(str, "Hello ", 50);
    memcpy2(str2, "World!", 50);
    strcat(str, str2);
    strncat(str, " Goodbye World!", 3);
    puts(str);
}

static void memcpy2(void * dest, const void * src, unsigned int n)
{
    char *csrc;
    char *cdest;
    unsigned int i; 

    csrc = (char *)src;
    cdest = (char *)dest;
    for (i = 0; i < n; i++)
    {
       cdest[i] = csrc[i]; 
    }
}

