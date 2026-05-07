#pragma safety enable


void* _Owner _Opt malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

struct X {
    char* _Owner name;
};

int main()
{
    //warning: cannot assign a temporary owner to non-owner object. 
    struct X* _Opt p = (struct X* _Owner _Opt) malloc(1); //lint 23
}




