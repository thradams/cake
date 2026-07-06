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


extern int vfprintf(struct _IO_FILE * __s, char * __format, __builtin_va_list __arg);
extern struct _IO_FILE * stdout;

int my_printf(char * fmt, ...)
{
    __builtin_va_list vl;
    int ret;

    __builtin_va_start(vl, fmt);
    ret = vfprintf(stdout, fmt, vl);
    __builtin_va_end(vl);
    return ret;
}


int main(void)
{
    my_printf("Rounding:\t%f %.0f %.32f\n", 1.5, 1.5, 1.3);
    my_printf("Padding:\t%05.2f %.2f %5.2f\n", 1.5, 1.5, 1.5);
    my_printf("Scientific:\t%E %e\n", 1.5, 1.5);
    my_printf("Hexadecimal:\t%a %A\n", 1.5, 1.5);
}


