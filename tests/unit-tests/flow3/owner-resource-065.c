#pragma safety enable

void* _Owner _Opt f();
void free(void* _Owner p);
int main()
{
    void* _Owner _Opt p  = f(); // owner object (p) not moved (call-opt-nonnull see line 11)
    if (p)
    {
        free(p);
        p = f();
    }
} //lint 29 owner object (p) not moved (call-opt-nonnull see line 11)
