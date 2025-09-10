#pragma flow enable

void f_const(const char* s);

void f_non_const(char* s);

int main(void)
{
    char buffer[20];

    //warning: uninitialized object 'buffer' passed to non-optional parameter
    [[cake::w30]] [[cake::w30]]
        f_const(buffer);

    f_non_const(buffer);
}