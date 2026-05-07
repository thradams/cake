#pragma safety enable

void* _Owner _Opt f();
void free(void* _Owner p);
int main()
{
    void* _Owner _Opt p  = f();
    if (p)
    {
        free(p);
        p = f();
    }
} //lint 29 memory leak
