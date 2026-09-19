#pragma safety enable


struct X {
    char* _Owner p;
};
void x_destroy(_Dtor struct X* p);

/* f delegates to x_destroy, so it must be _Dtor itself; a plain borrow that consumes the object is warning 72 */
void f(_Dtor struct X* x)
{

    x_destroy(x); // source expression of _Dtor must be addressof
}
