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
extern int fclose(struct _IO_FILE * __stream);
extern int fseek(struct _IO_FILE * __stream, int long __off, int __whence);
extern int fprintf(struct _IO_FILE * __stream, char * __format, ...);
extern struct _IO_FILE * stderr;
extern unsigned int long fread(void * __ptr, unsigned int long __size, unsigned int long __n, struct _IO_FILE * __stream);
extern int printf(char * __format, ...);

int main(void)
{
    double A[5];
    struct _IO_FILE * fp;
    double B[5];
    int ret_code;

    A[0] = 1.0;
    A[1] = 2.0;
    A[2] = 3.0;
    A[3] = 4.0;
    A[4] = 5.0;
    fp = fopen("test.bin", "wb");
    fwrite(A, 8L, 5, fp);
    fclose(fp);
    fp = fopen("test.bin", "rb");
    if (fseek(fp, 16L, 0) != 0)
    {
        fprintf(stderr, "fseek() failed in file %s at line # %d\n", "c:/users/thiag/source/repos/cake_private/tests/en-cpp-reference-c/ccode49.c", 20);
        fclose(fp);
        return 1;
    }
    ret_code = fread(B, 8L, 1, fp);
    printf("ret_code == %d\n", ret_code);
    printf("B[0] == %.1f\n", B[0]);
    fclose(fp);
    return 0;
}


