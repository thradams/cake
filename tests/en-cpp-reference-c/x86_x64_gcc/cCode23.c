/* Cake x86_x64_gcc */
struct _IO_codecvt;

struct _IO_wide_data;

struct _IO_marker;

struct _IO_FILE {
    int _flags;
    char * _IO_read_ptr;
    char * _IO_read_end;
    char * _IO_read_base;
    char * _IO_write_base;
    char * _IO_write_ptr;
    char * _IO_write_end;
    char * _IO_buf_base;
    char * _IO_buf_end;
    char * _IO_save_base;
    char * _IO_backup_base;
    char * _IO_save_end;
    struct _IO_marker * _markers;
    struct _IO_FILE * _chain;
    int _fileno;
    int _flags2;
    int long _old_offset;
    unsigned short _cur_column;
    signed char _vtable_offset;
    char _shortbuf[1];
    void * _lock;
    int long _offset;
    struct _IO_codecvt * _codecvt;
    struct _IO_wide_data * _wide_data;
    struct _IO_FILE * _freeres_list;
    void * _freeres_buf;
    unsigned int long __pad5;
    int _mode;
    char _unused2[20];
};


extern int *__errno_location(void);
extern int fputs(char * __s, struct _IO_FILE * __stream);
extern struct _IO_FILE * stdout;
extern int puts(char * __s);

void show_errno(void)
{
    char * err_info;

    err_info = "unknown error";
    /*switch*/
    {
        int __v0 = (*__errno_location());
        if (__v0 == 33) goto __L1; /*case 33*/
        if (__v0 == 84) goto __L2; /*case 84*/
        if (__v0 == 34) goto __L3; /*case 34*/
        if (__v0 == 0) goto __L4; /*case 0*/
        goto __L0;

        {
            __L1: /*case 33*/ 
            err_info = "domain error";
            goto __L0; /* break */

            __L2: /*case 84*/ 
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
    fputs(err_info, stdout);
    puts(" occurred");
}


extern double acos(double __x);
extern double log(double __x);
extern double sin(double __x);

int main(void)
{
    fputs("MATH_ERRNO is ", stdout);
    puts(1 ? "set" : "not set");
    (*__errno_location()) = 0;
    .7976931348623157E+308;
    show_errno();
    (*__errno_location()) = 0;
    acos(1.1000000000000001);
    show_errno();
    (*__errno_location()) = 0;
    log(0.0);
    show_errno();
    (*__errno_location()) = 0;
    sin(0.0);
    show_errno();
}


