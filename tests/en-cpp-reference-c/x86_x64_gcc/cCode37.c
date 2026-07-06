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
extern unsigned int long fread(void * __ptr, unsigned int long __size, unsigned int long __n, struct _IO_FILE * __stream);
extern int printf(char * __format, ...);
extern int fsetpos(struct _IO_FILE * __stream, struct _G_fpos_t * __pos);
extern struct _IO_FILE * stdin;
extern void perror(char * __s);

int main(void)
{
    struct _IO_FILE * fp;
    int rc;
    double __v0[4];
    struct _G_fpos_t  pos;
    double d;

    fp = fopen("test.bin", "wb");
    ;
    __v0[0] = 1.1000000000000001;
    __v0[1] = 2.2000000000000002;
    __v0[2] = 3.2999999999999998;
    __v0[3] = 4.4000000000000004;
    rc = fwrite(__v0, 8L, 4, fp);
    ;
    fclose(fp);
    fp = fopen("test.bin", "rb");
    fgetpos(fp, &pos);
    rc = fread(&d, 8L, 1, fp);
    ;
    printf("First value in the file: %.1f\n", d);
    fsetpos(fp, &pos);
    rc = fread(&d, 8L, 1, fp);
    ;
    printf("First value in the file again: %.1f\n", d);
    fclose(fp);
    rc = fsetpos(stdin, &pos);
    if (rc)
    {
        perror("could not fsetpos stdin");
    }
}


