#pragma safety enable

void free(void* _Owner _Opt p);
char* _Owner strdup(const char* s);
[[noreturn]] void exit( int exit_code );

void f()
{
    char * _Owner s = strdup("a");
    
    if (s == nullptr)
    {
        exit(1);
    }
    
    compile_assert(s != 0);
    free(s);
}
