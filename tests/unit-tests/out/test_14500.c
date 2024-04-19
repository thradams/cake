struct X { int i; void* p; };
void* /*_Owner*/ malloc(int i, int sz);
void free(void* /*_Owner*/ p);

int main()
{
    struct X* /*_Owner*/ p = malloc(1, 1);
    
    
    
    free(p);
}
