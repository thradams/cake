/* Cake x64_msvc */

static void memcpy(void * dest, const void * src, unsigned long long n);

int main()
{
    char s3[4];

    memcpy(s3, s2, 4);
}

static void memcpy(void * dest, const void * src, unsigned long long n)
{
    char *csrc;
    char *cdest;
    unsigned long long i; 

    csrc = (char *)src;
    cdest = (char *)dest;
    for (i = 0; i < n; i++)
    {
       cdest[i] = csrc[i]; 
    }
}

