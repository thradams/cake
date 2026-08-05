#pragma safety enable


struct X {
    char* _Owner p;
};
void x_destroy(_Dtor struct X* p);

/* f is genuinely a destructor for x (it delegates straight to x_destroy),
   so it must be declared _Dtor itself -- a plain borrow parameter that
   silently consumes the caller's object via a delegated dtor call is
   exactly the contract violation flow3_check_non_dtor_param_owner_not_consumed_at_exit
   exists to catch (warning 72). */
void f(_Dtor struct X* x)
{

    x_destroy(x); // source expression of _Dtor must be addressof
}
