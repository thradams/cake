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

union __tag2 {
    unsigned int __wch;
    char __wchb[4];
};

struct __tag1 {
    int __count;
    union __tag2  __value;
};

struct _G_fpos_t {
    int long __pos;
    struct __tag1  __state;
};


extern struct _IO_FILE *fopen(char * __filename, char * __modes);
extern unsigned int long fwrite(void * __ptr, unsigned int long __size, unsigned int long __n, struct _IO_FILE * __s);
extern int fclose(struct _IO_FILE * __stream);
extern int fgetpos(struct _IO_FILE * __stream, struct _G_fpos_t * __pos);
extern void perror(char * __s);
extern int fprintf(struct _IO_FILE * __stream, char * __format, ...);
extern struct _IO_FILE * stderr;
extern void exit(int __status);
extern unsigned int long fread(void * __ptr, unsigned int long __size, unsigned int long __n, struct _IO_FILE * __stream);
extern int printf(char * __format, ...);
extern int fsetpos(struct _IO_FILE * __stream, struct _G_fpos_t * __pos);
extern int ferror(struct _IO_FILE * __stream);

int main(void)
{
    double A[5];
    struct _IO_FILE * fp;
    double B[5];
    struct _G_fpos_t  pos;
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
    if (fgetpos(fp, &pos) != 0)
    {
        perror("fgetpos()");
        fprintf(stderr, "fgetpos() failed in file %s at line # %d\n", "c:/users/thiag/source/repos/cake_private/tests/en-cpp-reference-c/ccode50.c", 19);
        exit(1);
    }
    ret_code = fread(B, 8L, 1, fp);
    printf("%.1f; read count = %d\n", B[0], ret_code);
    if (fsetpos(fp, &pos) != 0)
    {
        if (ferror(fp))
        {
            perror("fsetpos()");
            fprintf(stderr, "fsetpos() failed in file %s at line # %d\n", "c:/users/thiag/source/repos/cake_private/tests/en-cpp-reference-c/ccode50.c", 30);
            exit(1);
        }
    }
    ret_code = fread(B, 8L, 1, fp);
    printf("%.1f; read count = %d\n", B[0], ret_code);
    fclose(fp);
    return 0;
}


