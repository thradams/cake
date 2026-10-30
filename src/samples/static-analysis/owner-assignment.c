#pragma safety enable

char * _Owner strdup(const char* s);
void free(void * _Nullable _Owner p);

int main()
{
    const char * _Owner s1 = strdup("hi");
    const char * _Owner s2 = nullptr;

    s2 = s1; /*moved*/

    free((void* _Owner)s2);
}
