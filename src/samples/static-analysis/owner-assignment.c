#pragma safety enable

char * _Opt _Owner strdup(const char* s);
void free(void * _Opt _Owner p);

int main()
{
    const char * _Owner _Opt s1 = strdup("hi");
    const char * _Owner _Opt s2 = nullptr;

    s2 = s1; /*moved*/

    free((void* _Owner)s2);
}
