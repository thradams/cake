#pragma safety enable


struct X {
    void* _Owner _Opt text;
};

void x_change(struct X* list);
void x_destroy(_Dtor struct X* p);

int main()
{
    //object pointed by 'x.text' was not released
    struct X x [[cake::leak]] = {};
    x_change(&x);
}


