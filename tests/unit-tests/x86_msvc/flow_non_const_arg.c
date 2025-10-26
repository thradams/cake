/* Cake 0.12.26 x86_msvc */
struct X {
    void * text;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void x_change(struct X * list);

int main()
{
    struct X  x;

    _cake_zmem(&x, 4);
    x_change(&x);
}


