#pragma safety enable

struct X { char* _Owner text; };

void param_view(_View struct X* p); //lint 2430
_View struct X* result_view(void); //lint 2430
_View struct X* global_view; //lint 2430

struct H {
    _View struct X* member; //lint 2430
    _View struct X object;
};

void local_view(struct X* p)
{
    _View struct X* q = p; //lint 2430
}

/* the object itself is the use of _View */
void by_view(_View struct X x);

void object_view(_View struct X x)
{
    _View struct X v = x;
    by_view(v);
    auto p = &v;
}
