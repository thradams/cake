#pragma safety enable



void free(void* _Owner _Opt p);

struct X {
    char* _Owner name;
};

void x_delete(struct X* _Owner _Opt p)
{
    if (p)
    {

        //object 'name' may not be empty
        [[cake::w29]]
        free(p);
    }
}


