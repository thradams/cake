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
extern int memcmp(void * __s1, void * __s2, unsigned int long __n);
extern int fputs(char * __s, struct _IO_FILE * __stream);
extern struct _IO_FILE * stdout;
extern int puts(char * __s);

void demo(char * lhs, char * rhs, unsigned int long sz)
{
    int rc;
    char * rel;

    {
        unsigned int long n;

        n = 0;
        for (; n < sz; ++n)
        putchar(lhs[n]);
    }
    rc = memcmp(lhs, rhs, sz);
    rel = rc < 0 ? " precedes " : rc > 0 ? " follows " : " compares equal ";
    fputs(rel, stdout);
    {
        unsigned int long n;

        n = 0;
        for (; n < sz; ++n)
        putchar(rhs[n]);
    }
    puts(" in lexicographical order");
}


int main(void)
{
    char a1[3];
    char a2[3];

    a1[0] = 97;
    a1[1] = 98;
    a1[2] = 99;
    a2[0] = 97;
    a2[1] = 98;
    a2[2] = 100;
    demo(a1, a2, 3L);
    demo(a2, a1, 3L);
    demo(a1, a1, 3L);
}


