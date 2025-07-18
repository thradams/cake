struct _iobuf {
    void * _Placeholder;
};


int *__cdecl _errno(void);
int __cdecl fputs(char * _Buffer, struct _iobuf * _Stream);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl puts(char * _Buffer);

void show_errno(void)
{
    char * err_info;

    err_info = "unknown error";
    /*switch*/
    {
        register int _R0 = (*_errno());
        if (_R0 == 33) goto _CKL1; /*case 33*/
        if (_R0 == 42) goto _CKL2; /*case 42*/
        if (_R0 == 34) goto _CKL3; /*case 34*/
        if (_R0 == 0) goto _CKL4; /*case 0*/
        goto _CKL0;

        {
            /*case 33*/ _CKL1:
            err_info = "domain error";
            /*break*/ goto _CKL0;

            /*case 42*/ _CKL2:
            err_info = "illegal sequence";
            /*break*/ goto _CKL0;

            /*case 34*/ _CKL3:
            err_info = "pole or range error";
            /*break*/ goto _CKL0;

            /*case 0*/ _CKL4:
            err_info = "no error";
        }
        _CKL0:;
    }
    fputs(err_info, (__acrt_iob_func(1)));
    puts(" occurred");
}

double __cdecl acos(double _X);
double __cdecl log(double _X);
double __cdecl sin(double _X);

int main(void)
{
    fputs("MATH_ERRNO is ", (__acrt_iob_func(1)));
    puts(1 ? "set" : "not set");
    (*_errno()) = 0;
    1.000000 / 0.000000;
    show_errno();
    (*_errno()) = 0;
    acos(1.100000);
    show_errno();
    (*_errno()) = 0;
    log(0.000000);
    show_errno();
    (*_errno()) = 0;
    sin(0.000000);
    show_errno();
}

