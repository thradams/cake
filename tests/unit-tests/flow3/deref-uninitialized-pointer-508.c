#pragma safety enable

/* issue #508 */
int f(void)
{
    int* p;
    return *p; //lint 30 using a possibly uninitialized object 'p'
}

void g(void)
{
    int* p;
    *p = 1; //lint 30 using a possibly uninitialized object 'p'
}
