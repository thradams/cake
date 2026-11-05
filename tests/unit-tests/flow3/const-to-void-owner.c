#pragma safety enable

/* const is not reported when converting to void* _Owner, only to a plain void* */

void free(void* _Owner _Nullable p);
void use(void* p);

void owner_parameter(const char* _Owner s)
{
    free(s);
}

void plain_parameter(const char* s)
{
    use(s); //lint 15
}

void plain_variable(const char* s)
{
    void* v = s; //lint 15
    (void)v;
}
