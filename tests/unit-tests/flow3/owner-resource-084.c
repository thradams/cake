#pragma safety enable


void free( void* _Owner ptr);
void* _Owner _Uninitialized malloc(int size);

int main()
{
    int* _Owner p = malloc(sizeof(int));
    if (p)
    {
       free(p);     
    }
}
