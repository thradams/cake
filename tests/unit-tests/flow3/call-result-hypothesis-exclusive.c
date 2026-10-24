#pragma safety enable

void* _Owner _Opt malloc(unsigned long n);
void free(void* _Owner _Opt p);

int* first(int* p);

/* the result is the value of p at the call, not the variable p */
int value_at_call(void)
{
    int* _Owner _Opt a = malloc(sizeof(int));
    int* _Owner _Opt b = malloc(sizeof(int));
    if (!a || !b)
    {
        free(a);
        free(b);
        return 0;
    }
    *a = 1;
    *b = 2;
    int* p = a;
    int* q = first(p);
    p = b;
    free(a);
    int v = *q; //lint 31
    free(b);
    return v;
}

/* p changed after the call: q was never b */
int not_the_new_value(void)
{
    int* _Owner _Opt a = malloc(sizeof(int));
    int* _Owner _Opt b = malloc(sizeof(int));
    if (!a || !b)
    {
        free(a);
        free(b);
        return 0;
    }
    *a = 1;
    *b = 2;
    int* p = a;
    int* q = first(p);
    p = b;
    free(b);
    int v = *q;
    free(a);
    return v;
}

/* a write through the result is on one object per path */
int write_one_side(void)
{
    int x = 1;
    int* q = first(&x);
    *q = 2;
    return 10 / (x - 1); //lint 36
}
