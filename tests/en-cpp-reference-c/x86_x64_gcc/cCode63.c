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


char str[20];
extern struct _IO_FILE *fopen(char * __filename, char * __modes);
extern int fputc(int __c, struct _IO_FILE * __stream);
extern int fclose(struct _IO_FILE * __stream);
extern unsigned int long fread(void * __ptr, unsigned int long __size, unsigned int long __n, struct _IO_FILE * __stream);
extern int puts(char * __s);
extern void rewind(struct _IO_FILE * __stream);

int main(void)
{
    struct _IO_FILE * f;
    char ch;

    f = fopen("file.txt", "w");
    for (ch = 48; ch <= 57; ch++)
    {
        fputc(ch, f);
    }
    fclose(f);
    f = fopen("file.txt", "r");
    fread(str, 1, 10, f);
    puts(str);
    rewind(f);
    fread(str, 1, 10, f);
    puts(str);
    fclose(f);
    return 0;
}


