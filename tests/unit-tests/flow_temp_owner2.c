#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

int main()
{
    //warning: cannot assign a temporary owner to non-owner object.
    [[cake::w23]]
    void* _Opt p = malloc(1);

    //warning: cannot assign a non-owner to owner 
    [[cake::w25]]
    free(p);
}



