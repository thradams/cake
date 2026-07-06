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


extern int printf(char * __format, ...);
extern struct _IO_FILE *fopen(char * __filename, char * __modes);
extern unsigned int long fwrite(void * __ptr, unsigned int long __size, unsigned int long __n, struct _IO_FILE * __s);
extern int fclose(struct _IO_FILE * __stream);
extern unsigned int long fread(void * __ptr, unsigned int long __size, unsigned int long __n, struct _IO_FILE * __stream);
extern int putchar(int __c);
extern int feof(struct _IO_FILE * __stream);
extern int ferror(struct _IO_FILE * __stream);
extern void perror(char * __s);

int main(void)
{
    double a[5];
    struct _IO_FILE * fp;
    double b[5];
    unsigned int long ret_code;

    a[0] = 1.0;
    a[1] = 2.0;
    a[2] = 3.0;
    a[3] = 4.0;
    a[4] = 5.0;
    printf("Array has size %ld bytes, element size: %ld\n", 40L, 8L);
    fp = fopen("test.bin", "wb");
    fwrite(a, 8L, 5, fp);
    fclose(fp);
    fp = fopen("test.bin", "rb");
    ret_code = fread(b, 8L, 5, fp);
    if (ret_code == 5)
    {
        printf("Array at %p read successfully, contents:\n", (void *)&a);
        {
            int n;

            n = 0;
            for (; n != 5; ++n)
            printf("%f ", b[n]);
        }
        putchar(10);
    }
    else
    {
        if (feof(fp))
        {
            printf("Error reading test.bin: unexpected end of file\n");
        }
        else
        {
            if (ferror(fp))
            {
                perror("Error reading test.bin");
            }
        }
    }
    fclose(fp);
}


