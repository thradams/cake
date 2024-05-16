#pragma safety enable

void free(void* _Owner _Opt p);
char* _Owner _Opt strdup(const char* s);
[[noreturn]] void exit( int exit_code );
#define NULL ((void*)0)
void f()
{
    char * _Opt _Owner s = strdup("a");
    
    if (s == NULL)    
        exit(1);    
    
    static_state(s, "not-null");
    static_debug(s);
    free(s);
}
