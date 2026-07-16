/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

static void * memset(void *dest, int ch, unsigned int count);

void F(char * s)
{
}

int main()
{
    char buffer[2];

    memset(&buffer, 0, 2);
    F(buffer + 1);
}

static void * memset(void *ptr, int value, unsigned int count)
{
    unsigned char *p;
    unsigned char v;

    p = (unsigned char *) ptr;
    v = (unsigned char) value;
    while (count--)
    {
        *p++ = v;
    }

    return ptr;
}
