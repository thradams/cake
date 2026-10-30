#pragma safety enable

void* _Owner malloc(unsigned long n);
void free(void* _Owner _Nullable p);

int* find(int* p);
int* find_restrict(int* restrict p);

/* the _Nullable result may be the argument */
int opt_result_is_argument(void)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p) return 0;
    *p = 1;
    int* q = find(p);
    free(p);
    if (q)
    {
        return *q; //lint 31
    }
    return 0;
}

/* no use after free: nothing to report */
int opt_used_before_free(void)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p) return 0;
    *p = 1;
    int* q = find(p);
    int v = q ? *q : 0;
    free(p);
    return v;
}

/* a restrict parameter is not returned */
int opt_restrict_parameter(void)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p) return 0;
    *p = 1;
    int* q = find_restrict(p);
    free(p);
    if (q)
    {
        return *q;
    }
    return 0;
}
