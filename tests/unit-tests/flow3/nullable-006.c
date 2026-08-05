#pragma nullable enable

struct X { char* name; };

struct X return_struct()
{
    struct X x;
    return x; //lint 30 passing a possible uninitialized object 'x.name' (see line 7)
}
