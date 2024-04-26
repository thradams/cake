#pragma safety enable



void free(void* /*_Owner*/ /*_Opt*/ p);

struct X {
    char* /*_Owner*/ name;
};

void x_delete(struct X* /*_Owner*/ /*_Opt*/ p)
{
    if (p) {
        //free(p->name);
        free(p);
    }
}

#pragma cake diagnostic check "-Wmissing-destructor"

