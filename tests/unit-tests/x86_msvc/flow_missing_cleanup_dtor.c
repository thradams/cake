/* Cake 0.12.26 x86_msvc */
struct X {
    char * name;
};


void free(void * p);

void x_delete(struct X * p)
{
    if (p)
    {
        free(p);
    }
}


