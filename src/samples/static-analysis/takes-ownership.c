#pragma safety enable

void * _Opt _Owner _Clear calloc(unsigned long n, unsigned long s);
char * _Opt _Owner strdup(const char* s);
void free(void * _Opt _Owner p);
void printf(char char* fmt, ...);

void takes_ownership(char * _Owner _Opt some_string)
{
    if (some_string)
    {
        printf("%s", some_string);
        free(some_string);
    }
}

int main()
{
    _Opt _Owner auto s = strdup("hello");
    takes_ownership(s /*moved here*/ );
}
