#pragma safety enable

struct Y {
    struct X* _Owner _Opt y;
};

void y_destroy([[dtor]] struct X* p)
{
    //p->s not released!    
}

struct X {
    char* _Owner _Opt s;
};
