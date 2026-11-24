/* github.com/thradams/cake/issues/523 */

#pragma safety enable

void get(char * _Out _Owner _Nullable * p);
void free(void * _Owner _Nullable p);

void f1()
{
    char * _Owner s;
    get(&s);
    free(s);
}

void f2()
{
    char * _Owner s;
    get(&s);
    free(s);
}
