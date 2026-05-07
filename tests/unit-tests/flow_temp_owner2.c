#pragma safety enable

void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

int main()
{
    //warning: cannot assign a temporary owner to non-owner object.
    void* _Opt p = malloc(1); //lint 23

    //warning: cannot assign a non-owner to owner 
    free(p); //lint 25
}



