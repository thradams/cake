#pragma safety enable

void g(int argc, char** argv)
{
    for (int i = 1; i < argc; i++)
    {
        char* _Opt s = argv[i] + 2;
        compile_assert(s != 0);
        compile_assert(argv[i] != 0);
    }
}
