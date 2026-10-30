#pragma safety enable

_Clear void * _Owner calloc(unsigned long n, unsigned long s);
char * _Owner strdup(const char* s);
void free(void * _Nullable _Owner p);
void printf(char char* fmt, ...);

void takes_ownership(char * _Owner _Nullable some_string)
{
    if (some_string)
    {
        printf("%s", some_string);
        free(some_string);
    }
}

int main()
{
    _Owner auto s = strdup("hello");
    takes_ownership(s /*moved here*/ );
}
