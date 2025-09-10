#pragma safety enable


char* _Opt f()
{
    char* _Owner _Opt p = 0;
    //error: cannot return a automatic storage duration _Owner to non-owner
    [[cake::e1280]]
    return p;
}
