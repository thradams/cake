
void *__cdecl malloc(unsigned int _Size);
void __cdecl free(void * _Block);
void *__cdecl calloc(unsigned int _Count, unsigned int _Size);
void *__cdecl realloc(void * _Block, unsigned int _Size);

int main(void)
{
    int * p1;
    int * p2;
    int * p3;

    p1 = malloc(40);
    free(p1);
    p2 = calloc(10, 4U);
    p3 = realloc(p2, 4000);
    if (p3)
    {
        free(p3);
    }
    else
    {
        free(p2);
    }
}

