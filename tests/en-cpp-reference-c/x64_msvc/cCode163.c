/* Cake x64_msvc */

void *__cdecl malloc(unsigned long long _Size);
void __cdecl free(void * _Block);
void *__cdecl calloc(unsigned long long _Count, unsigned long long _Size);
void *__cdecl realloc(void * _Block, unsigned long long _Size);

int main(void)
{
    int * p1;
    int * p2;
    int * p3;

    p1 = malloc(40ULL);
    free(p1);
    p2 = calloc(10, 4ULL);
    p3 = realloc(p2, 4000ULL);
    if (p3)
    {
        free(p3);
    }
    else
    {
        free(p2);
    }
}
