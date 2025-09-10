#pragma safety enable

struct X {
    char* _Owner _Opt name;
};

struct X* _Owner make();
void del(struct X* _Owner _Opt p);

int main()
{
    struct X* _Owner p = make();

    //error: operator ++ cannot be used in _Owner pointers
    [[cake::e1310]]
    p++;

    //error: operator -- cannot be used in _Owner pointers
    [[cake::e1320]]
    p--;

    del(p);
}


