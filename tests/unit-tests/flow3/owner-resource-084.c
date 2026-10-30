#pragma safety enable


void free( void* _Owner ptr);
_Uninitialized void* _Nonnull _Owner malloc(int size);

int main()
{
    int* _Owner p = malloc(sizeof(int));
    if (p)
    {
       free(p);     
    }
}
