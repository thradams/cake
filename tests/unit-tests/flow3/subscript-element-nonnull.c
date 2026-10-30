#pragma safety enable

void g(int argc, char* _Nonnull const* argv)
{
    for (int i = 1; i < argc; i++)
    {
        char* s = argv[i] + 2;
        compile_assert(s != 0);
        compile_assert(argv[i] != 0);
    }
}
