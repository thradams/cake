#pragma safety enable

void free(void* _Owner _Opt p);
char* _Owner _Opt strdup(const char* s);
[[noreturn]] void exit( int exit_code );

void f()
{
    char * _Opt _Owner s = strdup("a");
    
    if (s == nullptr)
    {
        exit(1);
    }
    
    static_state(s, "not-null");
    free(s);
}
