#pragma safety enable


_Uninitialized void* _Owner malloc(int i);
void free(void* _Owner _Opt p);
int rand();

int main()
{
    char* _Owner s = malloc(1); // owner object (s) not moved (call-opt-nonnull see line 10)
    try
    {
        if (rand())
        {
            free(s);
        }
        else
        {
            throw;
        }
    }
    catch
    {
    }
} //lint 29 owner object (s) not moved (call-opt-nonnull see line 10)
