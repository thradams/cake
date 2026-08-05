#pragma safety enable

void f(void)
{
    char new_file[512] = "";
    compile_assert(new_file[0] == 0);
    compile_assert(new_file[1] == 0);
    compile_assert(new_file[511] == 0);
}
