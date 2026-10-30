#pragma safety enable

/* issue 488: ignoring an _Owner result is reported also when the call is through a function pointer */

char* _Owner strdup(const char* s);
void free(void* _Owner _Opt ptr);

char* _Owner make(void)
{
    return strdup("a");
}

int main(void)
{
    char* _Owner _Opt (*f)(void) = make;
    make(); //lint 9
    f(); //lint 9
    char* _Owner s = f();
    free(s);
    (void)f;
}
