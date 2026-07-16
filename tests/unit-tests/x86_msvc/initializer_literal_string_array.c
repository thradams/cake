/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

static void* memcpy(void * dest, const void * src, unsigned int n);

void T6()
{
    char s[4];
    char s2[4];

    memcpy(s, "123", 4);
    memcpy(s2, s, 4);
}

static void* memcpy(void * dest, const void * src, unsigned int n)
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

