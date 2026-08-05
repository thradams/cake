#pragma safety enable

struct X {
    int* _Opt pi;
};

int* f();
int main(struct X* _Opt pX)
{
    if (pX)
    {
        compile_assert(pX != 0);
    }
    else
    {
        compile_assert(pX == 0);
    }

    if (!pX)
    {
        compile_assert(pX == 0);
    }
    else
    {
        compile_assert(pX != 0);
    }

    if (pX && pX->pi)
    {
        compile_assert(pX != 0);
        compile_assert(pX->pi != 0);
    }
    else
    {
        //static_debug(pX);
        //assert_state(pX, "not-null null");
        //assert_state(pX->pi, "not-null null");
    }

    // warning: pointer is always non-null 

    if (pX == 0 || (pX->pi = f()) == 0)
    {
        //assert_state(pX, "null not-null");
    }
    else
    {
        //assert_state(pX, "not-null"); //FAILS only on github windows!? WTF
    }


}
