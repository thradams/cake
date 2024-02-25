void* _Owner malloc(int sz);
void free(void* _Owner _Opt p);


void f(int i) {
    void* _Owner p = 0;
    if (i)
    {
        if (i)
        {
            p = malloc(1);
        }
        else
        {
            p = malloc(1);
        }
    }

    free(p);
}


