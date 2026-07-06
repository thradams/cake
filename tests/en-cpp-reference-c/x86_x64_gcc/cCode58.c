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


extern int putchar(int __c);
extern int ferror(struct _IO_FILE * __stream);
extern struct _IO_FILE * stdout;
extern int fprintf(struct _IO_FILE * __stream, char * __format, ...);
extern struct _IO_FILE * stderr;
extern void perror(char * __s);
extern void exit(int __status);
extern int printf(char * __format, ...);

int main(void)
{
    int ret_code;
    int r;

    ret_code = 0;
    {
        char c;

        c = 97;
        for (; (ret_code != -1) && (c != 122); c++)
        ret_code = putchar(c);
    }
    if (ret_code == -1 && ferror(stdout))
    {
        fprintf(stderr, "putchar() failed in file %s at line # %d\n", "c:/users/thiag/source/repos/cake_private/tests/en-cpp-reference-c/ccode58.c", 9);
        perror("putchar()");
        exit(1);
    }
    putchar(10);
    r = 4208;
    printf("\n0x%x\n", r);
    r = putchar(r);
    printf("\n0x%x\n", r);
}


