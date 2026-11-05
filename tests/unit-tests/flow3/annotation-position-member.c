#pragma safety enable

struct Y { int i; };

struct X {
    _Dtor struct Y* a; //lint 2440
    _Uninitialized struct Y* b; //lint 2450
    struct Y* c;
};

void take(_Dtor struct Y* p);
_Uninitialized struct Y* make(void);
