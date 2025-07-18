
static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void F(char * s)
{
}

int main()
{
    char buffer[2];

    _cake_zmem(&buffer, 2);
    F(buffer + 1);
}

