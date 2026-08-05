#pragma safety enable


void* _Owner _Uninitialized malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

int main()
{
    void* _Owner _Opt p = 0;
    for (int i = 0; i < 2; i++)
    {

        //object 'p' may not be empty
        
        p = malloc(1); //lint 26 assignment discards _Owner 'p' without releasing it first (see line 15)

    }
    free(p);
}
