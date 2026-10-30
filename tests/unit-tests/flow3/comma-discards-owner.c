#pragma safety enable

/* The comma's left operand is discarded: an _Owner result is lost. */

void* _Owner malloc(int n);
void free(void* _Owner _Nullable p);

void f1(void)
{
    void * _Owner p = (malloc(1), malloc(2)); //lint 26
    free(p);
}

void f2(void)
{
    /* no warning: right operand is moved to p */
    int i = 0;
    void * _Owner p = (i++, malloc(2));
    free(p);
}
