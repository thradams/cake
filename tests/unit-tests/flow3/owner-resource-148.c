#pragma safety enable


_Uninitialized void* _Owner malloc(unsigned long size);
void free(void* _Owner _Opt ptr);

struct X {
    char* _Nonnull _Owner name;
};

int main()
{
    //warning: cannot assign a temporary owner to non-owner object. 
    struct X* p = (struct X* _Owner) malloc(1); //lint 79 30 temporary owner to non-owner; pointee uninitialized
}
