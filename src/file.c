void free( void* owner ptr);
void* owner malloc(int size);
struct X { char * owner text; };

void x_destroy(struct X* obj_owner p);

void x_delete(struct X* owner p)
{
    if (p)
    {
        x_destroy(p);
        free(p);
    }
}