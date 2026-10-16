#pragma safety enable

// a ?: moves its _Owner arm only when an _Owner destination takes the result;
// passing it to a non-owner parameter is a borrow, even when both arms are owners.

void* _Owner _Opt malloc(unsigned long n);
void free(void* _Owner _Opt p);
void use(const int* _Opt p);
void sink(int* _Owner _Opt p);
int* _Owner _Opt make(void);

void both_owner_arms_borrowed(int c)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    int* _Owner _Opt q = malloc(sizeof(int));
    use(c ? p : q);
    free(p);
    free(q);
}

void nested_borrowed(int a, int b)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    int* _Owner _Opt q = malloc(sizeof(int));
    use(a ? (b ? p : q) : 0);
    free(p);
    free(q);
}

void moved_into_owner_param(int c)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    int* _Owner _Opt q = malloc(sizeof(int));
    sink(c ? p : q);
    free(p); //lint 32
    free(q); //lint 32
}

void moved_by_assignment(int c)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    int* _Owner _Opt r = 0;
    r = c ? p : 0;
    free(r);
} //lint 29

int* _Owner _Opt moved_by_return(int c)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    int* _Owner _Opt q = malloc(sizeof(int));
    return c ? p : q; //lint 29 29
}
