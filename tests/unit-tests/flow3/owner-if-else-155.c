#pragma safety enable


_Uninitialized void* _Nonnull _Owner malloc(unsigned size);
void free(void* _Owner ptr);

struct X {
    char* _Nonnull _Owner name;
};

struct X* _Owner F(int i)
{

    struct X* _Owner p1 = 0;
    try
    {
        if (i == 1)
        {
            p1 = malloc(sizeof * p1); // sizeof operand unevaluated: no null deref
            while (0) //lint 44
            {}
        }
        else if (i == 3)
        {
            p1 = malloc(sizeof * p1); // sizeof operand unevaluated: no null deref
        }
    }
    catch
    {
    }

    //warning: uninitialized object 'p1.name'
    return p1; // p1.name not initialized
}
