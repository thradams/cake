struct _struct_tag_1 {
    int * pSelect;
};

union _struct_tag_0 {
    struct _struct_tag_1  view;
};

struct X {
    union _struct_tag_0  u;
};


int main()
{
    struct X  t;

    t.u.view.pSelect = 0;
}

