#pragma safety enable

void f_const(const char* s);

void f_non_const(char* s);

int main(void)
{
    char buffer[20];

    //warning: uninitialized object 'buffer' passed to non-optional parameter

    f_const(buffer); //lint 30 

    //static_debug_ex(buffer);

    //warning: uninitialized object 'buffer' passed to non-optional parameter

    f_non_const(buffer); //lint 30
}
