#pragma safety enable

void* _Owner _Opt calloc(unsigned long n, unsigned long s);
struct node { int* first; };
void node_delete(_Dtor struct node* _Owner _Opt p);
int* get(void);
int fail(void);

struct node* _Owner _Opt make(void)
{
    struct node* _Owner _Opt p = 0;
    try
    {
        p = calloc(1, sizeof *p);
        if (p == 0) throw;
        p->first = get();
        if (fail()) throw;
    }
    catch
    {
        node_delete(p);
        p = 0;
    }
    _Assert(p == 0 || p->first); /* no warning 30: p->first is uninitialized only where p is 0 */
    return p;
}
