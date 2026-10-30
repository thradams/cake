#pragma safety enable

struct X {
    char* _Owner name;
};

struct X* _Nonnull _Owner make();
void del(struct X* _Owner _Opt p);

int main()
{
    struct X* _Owner p = make();

    //error: operator ++ cannot be used in _Owner pointers

    p++; //lint 1310 operator ++ cannot be used in _Owner pointers

    //error: operator -- cannot be used in _Owner pointers

    p--; //lint 1320 operator -- cannot be used in _Owner pointers

    del(p);
}
