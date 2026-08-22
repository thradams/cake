#pragma safety enable

void* _Owner _Opt _Uninitialized malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

int main()
{
    //warning: cannot assign a temporary owner to non-owner object.
    void* _Opt p = malloc(1); //lint 79 cannot assign a temporary owner to non-owner object.

    //warning: cannot assign a non-owner to owner 
    free(p); //lint 78 cannot assign a non-owner to owner
}
