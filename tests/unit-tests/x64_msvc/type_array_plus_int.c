/* Cake x64_msvc */

static void memset(void *dest, int ch, unsigned long long count);

void F(char * s)
{
}

int main()
{
    char buffer[2];

    memset(&buffer, 0, 2);
    F(buffer + 1);
}

static void memset(void *dest, int ch, unsigned long long count)
{
    unsigned char *ptr;

    ptr = (unsigned char*)dest;
    while (count-- > 0)
    {
       *ptr++ = 0;
    }
}

