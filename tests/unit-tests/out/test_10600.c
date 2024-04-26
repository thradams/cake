
#pragma safety enable



void free( void* /*_Owner*/ ptr);
void* /*_Owner*/ malloc(int size);
struct X { char * /*_Owner*/ text; };

void x_destroy(struct X* /*_Obj_owner*/ p)
{
    free(p->text);
}

void x_delete(struct X* /*_Owner*/ /*_Opt*/ p)
{
    if (p)
    {
        x_destroy(p);
        free(p);
    }
}