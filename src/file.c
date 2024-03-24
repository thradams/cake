void* _Owner malloc(int i);
void free(void* _Owner p);

struct X {
    char* _Owner name;
};

void x_delete(struct X* _Owner p)
{
    if (p) {
        //free(p->name);
        free(p);
    }
}

#pragma cake diagnostic check "-Wmissing-destructor"
