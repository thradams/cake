
void *calloc(int i, int n);
void free(void * p);

int main()
{
    int * p1;
    int * p2;

    p1 = 0;
    p2 = calloc(1, 4U);
    if (p2 == 0)
    {
        return 1;
    }
    else
    {
        p1 = p2;
    }
    free(p1);
    return 0;
}

