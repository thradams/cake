#pragma safety enable


void free( void* _Owner ptr);
void* _Owner malloc(int size);

int main()
{
    int* _Owner p = malloc(sizeof(int));
    if (p != 0)
    {
       free(p);     
    }
}

