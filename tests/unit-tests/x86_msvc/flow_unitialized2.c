/* Cake 0.12.26 x86_msvc */

void f_const(char * s);
void f_non_const(char * s);

int main(void)
{
    char buffer[20];

    f_const(buffer);
    f_non_const(buffer);
}


