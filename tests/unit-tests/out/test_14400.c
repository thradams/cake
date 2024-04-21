#pragma nullable enable
#pragma ownership enable


struct X { int i; void* p; };
void* /*_Owner*/ calloc(int i, int sz);
void free(void* /*_Owner*/ p);

int main()
{
    struct X* /*_Owner*/ p = calloc(1, 1);
    
    
    
    free(p);
}
