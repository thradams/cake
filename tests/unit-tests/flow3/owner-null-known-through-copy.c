#pragma safety enable

void* _Owner malloc(unsigned long n);
void free(void* _Owner _Nullable p);

void chained_assignment_in_condition(void)
{
    int* _Owner p;
    int* q;
    if ((q = p = malloc(sizeof(int))) == 0)
    {
        return; /* p is null here */
    }
    free(p);
}

void copy_tested_later(void)
{
    int* _Owner p = malloc(sizeof(int));
    int* q = p;
    if (q == 0)
    {
        return; /* p is null here */
    }
    free(p);
}

void copy_non_null_side_leaks(void)
{
    int* _Owner p = malloc(sizeof(int));
    int* q = p;
    if (q != 0)
    {
        return; //lint 29
    }
}
