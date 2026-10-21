/* github.com/thradams/cake/issues/523 */

#pragma safety enable

void get(char * _Out _Owner _Opt * p);
void free(void * _Owner _Opt p);

void f1()
{
    char * _Owner s;
    get(&s); //lint 54
    free(s);
}

void f2()
{
    char * _Owner _Opt s;
    get(&s);
    free(s);
}
