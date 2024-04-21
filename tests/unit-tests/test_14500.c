#pragma nullable enable
#pragma ownership enable

struct X { int i; void* p; };
void* _Owner malloc(int i, int sz);
void free(void* _Owner p);

int main()
{
    struct X* _Owner p = malloc(1, 1);
    static_state(p, "maybe-null ");
    static_state(p->i, "uninitialized");
    static_state(p->p, "uninitialized");
    free(p);
}
