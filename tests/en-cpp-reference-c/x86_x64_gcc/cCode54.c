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


extern struct _IO_FILE *fopen(char * __filename, char * __modes);
extern unsigned int long fwrite(void * __ptr, unsigned int long __size, unsigned int long __n, struct _IO_FILE * __s);
extern int printf(char * __format, ...);
extern int fclose(struct _IO_FILE * __stream);
extern unsigned int long fread(void * __ptr, unsigned int long __size, unsigned int long __n, struct _IO_FILE * __stream);

int main(void)
{
    double a[5];
    struct _IO_FILE * f1;
    unsigned int long r1;
    double b[5];
    struct _IO_FILE * f2;
    unsigned int long r2;

    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    a[3] = 4;
    a[4] = 5;
    f1 = fopen("file.bin", "wb");
    ;
    r1 = fwrite(a, 8L, 5, f1);
    printf("wrote %zu elements out of %d requested\n", r1, 5);
    fclose(f1);
    f2 = fopen("file.bin", "rb");
    r2 = fread(b, 8L, 5, f2);
    fclose(f2);
    printf("read back: ");
    {
        unsigned int long i;

        i = 0;
        for (; i < r2; ++i)
        printf("%0.2f ", b[i]);
    }
}


