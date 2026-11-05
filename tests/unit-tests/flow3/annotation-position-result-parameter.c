#pragma safety enable

struct X { int i; };

_Dtor struct X* result_dtor(void); //lint 2410
void param_dtor(_Dtor struct X* p);

void local_dtor(struct X* p)
{
    _Dtor struct X* q = p; //lint 2410
}

void param_uninit(_Uninitialized struct X* p); //lint 2420
_Uninitialized struct X* result_uninit(void);

void local_uninit(struct X* p)
{
    _Uninitialized struct X* q = p; //lint 2420
}

/* auto inherits _Uninitialized from the result */
_Uninitialized struct X* _Nonnull result_uninit_nonnull(void);

void local_auto(void)
{
    auto q = result_uninit_nonnull();
}
