#pragma safety enable
#pragma flow enable

struct X { char* _Nonnull name; };

struct X * _Nonnull f()
{
    struct X* p;
    {
        struct X x;
        p = &x; //lint 30 passing a possible uninitialized object '(*&x).name' (see line 9)

    }
    
    return p; //lint 31 : passing a possible uninitialized object '(*&x).name' (see line 9)

}

