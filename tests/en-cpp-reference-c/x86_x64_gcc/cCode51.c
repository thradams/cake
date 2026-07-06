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


extern void perror(char * __s);
extern int fprintf(struct _IO_FILE * __stream, char * __format, ...);
extern struct _IO_FILE * stderr;
extern void exit(int __status);

void check(unsigned char  condition, char * func, int line)
{
    if (condition)
    {
        return;
    }
    perror(func);
    fprintf(stderr, "%s failed in file %s at line # %d\n", func, "c:/users/thiag/source/repos/cake_private/tests/en-cpp-reference-c/ccode51.c", line - 1);
    exit(1);
}


extern struct _IO_FILE *fopen(char * __filename, char * __modes);
extern unsigned int long fwrite(void * __ptr, unsigned int long __size, unsigned int long __n, struct _IO_FILE * __s);
extern int fclose(struct _IO_FILE * __stream);
extern int long ftell(struct _IO_FILE * __stream);
extern int printf(char * __format, ...);
extern unsigned int long fread(void * __ptr, unsigned int long __size, unsigned int long __n, struct _IO_FILE * __stream);

int main(void)
{
    double A[5];
    char * fname;
    struct _IO_FILE * file;
    int write_count;
    double B[5];
    int long pos;
    int read_count;

    A[0] = 1.1000000000000001;
    A[1] = 2.0;
    A[2] = 3.0;
    A[3] = 4.0;
    A[4] = 5.0;
    fname = "/tmp/test.bin";
    file = fopen(fname, "wb");
    check(file != 0, "fopen()", 24);
    write_count = fwrite(A, 8L, 5, file);
    check(write_count == 5, "fwrite()", 27);
    fclose(file);
    file = fopen(fname, "rb");
    check(file != 0, "fopen()", 34);
    pos = ftell(file);
    check(pos != -1L, "ftell()", 37);
    printf("pos: %ld\n", pos);
    read_count = fread(B, 8L, 1, file);
    check(read_count == 1, "fread()", 41);
    pos = ftell(file);
    check(pos != -1L, "ftell()", 44);
    printf("pos: %ld\n", pos);
    printf("B[0]: %.1f\n", B[0]);
    return 0;
}


