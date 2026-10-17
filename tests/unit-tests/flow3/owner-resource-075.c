#pragma safety enable


struct X
{
    int i;
    void *p;
};
_Uninitialized void *_Owner _Opt malloc(int i, int sz);
void free(void *_Owner _Opt p);

int main()
{
    struct X *_Owner _Opt p = malloc(1, sizeof * p);
      if (p)
    {
    }
    free(p);
}
