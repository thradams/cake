#pragma safety enable

// a ?: with an _Owner arm and a non-owner arm is a borrow: passing it to a
// non-owner parameter does not move the owner (issue 491 still moves for "p : 0").

void* _Owner _Opt malloc(unsigned long n);
void free(void* _Owner _Opt p);
void use(const int* _Opt p);

void f(int* _Opt tail)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    use(tail != 0 ? tail : p);
    free(p);
}

void g(int c)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    int* _Owner _Opt q = c ? p : 0;
    free(q);
} //lint 29
