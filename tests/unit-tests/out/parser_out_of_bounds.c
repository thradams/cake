
static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

int main()
{
    int a[5];
    int i;

    _cake_zmem(&a, 20);
    i = a[5];
}

