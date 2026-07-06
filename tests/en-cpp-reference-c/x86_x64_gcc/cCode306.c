/* Cake x86_x64_gcc */

extern int system(char * __command);

int main(void)
{
    system("date +%A");
    system("gcc --version");
}


