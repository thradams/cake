/* Cake 0.12.26 x86_msvc */
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
        int __v0 = (*_errno());
        if (__v0 == 33) goto __L1; /*case 33*/
        if (__v0 == 42) goto __L2; /*case 42*/
        if (__v0 == 34) goto __L3; /*case 34*/
        if (__v0 == 0) goto __L4; /*case 0*/
        goto __L0;

        {
            __L1: /*case 33*/ 
            err_info = "domain error";
            goto __L0; /* break */

            __L2: /*case 42*/ 
            err_info = "illegal sequence";
            goto __L0; /* break */

            __L3: /*case 34*/ 
            err_info = "pole or range error";
            goto __L0; /* break */

            __L4: /*case 0*/ 
            err_info = "no error";
        }
        __L0:;
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
    -nan(ind).0;
    show_errno();
    (*_errno()) = 0;
    acos(1.1000000000000001);
    show_errno();
    (*_errno()) = 0;
    log(0.0);
    show_errno();
    (*_errno()) = 0;
    sin(0.0);
    show_errno();
}


