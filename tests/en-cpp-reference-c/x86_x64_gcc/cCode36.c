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
extern void perror(char * __s);
extern int fputs(char * __s, struct _IO_FILE * __stream);
extern void rewind(struct _IO_FILE * __stream);
extern int fgetc(struct _IO_FILE * __stream);
extern int putchar(int __c);
extern int ferror(struct _IO_FILE * __stream);
extern int puts(char * __s);
extern int feof(struct _IO_FILE * __stream);
extern int fclose(struct _IO_FILE * __stream);
extern int remove(char * __filename);

int main(void)
{
    char * fname;
    int is_ok;
    struct _IO_FILE * fp;
    int c;

    fname = "/tmp/unique_name.txt";
    is_ok = 1;
    fp = fopen(fname, "w+");
    if (!fp)
    {
        perror("File opening failed");
        return is_ok;
    }
    fputs("Hello, world!\n", fp);
    rewind(fp);
    while ((c = fgetc(fp)) != -1)
    putchar(c);
    if (ferror(fp))
    {
        puts("I/O error when reading");
    }
    else
    {
        if (feof(fp))
        {
            puts("End of file is reached successfully");
            is_ok = 0;
        }
    }
    fclose(fp);
    remove(fname);
    return is_ok;
}


