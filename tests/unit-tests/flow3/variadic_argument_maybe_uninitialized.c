#pragma safety enable

int printf(char * format, ...);

void f(int c)
{
    int j;
    if (c)
      j = 1;
    printf("%d", j); //lint 30 warning
}

int * _Owner _Opt make(void);
void free(void * _Owner _Opt p);
void f2(void)
{
    int * _Owner _Opt p = make();
    printf("%p", p);
    free(p);
}
