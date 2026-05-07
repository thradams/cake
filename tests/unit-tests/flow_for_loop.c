#pragma safety enable


void* _Owner malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

int main()
{
    void* _Owner _Opt p = 0;
    for (int i = 0; i < 2; i++)
    {

        //object 'p' may not be empty
        
        p = malloc(1); //lint 29

    }
    free(p);
}