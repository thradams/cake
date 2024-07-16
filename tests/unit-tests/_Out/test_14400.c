#pragma safety enable



struct X { int i; void* p; };
void* /*_Owner*/ /*_Opt*/ calloc(int i, int sz);
void free(void* /*_Owner*/ /*_Opt*/ p);

int main()
{
    struct X* /*_Owner*/ /*_Opt*/ p = calloc(1, 1);
    
    if (p)
    {
    
    
    }
    free(p);
}
