/* Cake x86_msvc */
struct X {
    void * text;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void f(struct X * list);

int main()
{
    struct X  x;

    _cake_zmem(&x, 4);
    f(&x);
}


