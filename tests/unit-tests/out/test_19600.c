#pragma safety enable

void free(void* /*_Owner*/ /*_Opt*/ p);
char* /*_Owner*/ /*_Opt*/ strdup(const char* s);
 void exit( int exit_code );

void f()
{
    char * /*_Opt*/ /*_Owner*/ s = strdup("a");
    
    if (s == ((void*)0))
    {
        exit(1);
    }
    
    
    free(s);
}
