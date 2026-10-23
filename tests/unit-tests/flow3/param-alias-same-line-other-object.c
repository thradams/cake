#pragma safety enable

/* the normal analysis reports r on the line; the aliasing problem with p2 on the same line, same id, is still reported */

void free(void* _Owner _Opt p);

struct X { int i; };

int f(struct X* _Owner p, const struct X* p2, struct X* _Owner r)
{
    free(r);
    free(p);
    return r->i + p2->i; //lint 31 100
}
