#pragma safety enable

void* _Owner _Opt malloc(unsigned long n);
void free(void* _Owner _Opt p);

int* _Opt find(int* p);
int* _Opt find_restrict(int* restrict p);

/* the _Opt result may be the argument */
int opt_result_is_argument(void)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    if (!p) return 0;
    *p = 1;
    int* _Opt q = find(p);
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
    int* _Owner _Opt p = malloc(sizeof(int));
    if (!p) return 0;
    *p = 1;
    int* _Opt q = find(p);
    int v = q ? *q : 0;
    free(p);
    return v;
}

/* a restrict parameter is not returned */
int opt_restrict_parameter(void)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    if (!p) return 0;
    *p = 1;
    int* _Opt q = find_restrict(p);
    free(p);
    if (q)
    {
        return *q;
    }
    return 0;
}
