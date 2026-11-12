#pragma safety enable


struct X
{
    int i;
    void * _Nonnull p;
};
_Uninitialized void *_Owner malloc(int i, int sz);
void free(void *_Owner _Opt p);

int main()
{
    struct X *_Owner p = malloc(1, sizeof * p);
      if (p)
    {
    }
    free(p);
}
