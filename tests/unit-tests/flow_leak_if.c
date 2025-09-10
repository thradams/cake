#pragma safety enable

void* _Owner _Opt f();
void free(void* _Owner p);
int main()
{
    void* _Owner _Opt p [[cake::leak]] = f();
    if (p)
    {
        free(p);
        p = f();
    }
}
