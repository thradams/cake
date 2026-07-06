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


extern char *setlocale(int __category, char * __locale);
extern struct _IO_FILE *fopen(char * __filename, char * __modes);
extern void perror(char * __s);
extern int fputs(char * __s, struct _IO_FILE * __stream);
extern int fclose(struct _IO_FILE * __stream);
extern int *__errno_location(void);
extern unsigned int fgetwc(struct _IO_FILE * __stream);
extern unsigned int putwchar(int __wc);
extern int ferror(struct _IO_FILE * __stream);
extern int puts(char * __s);
extern int feof(struct _IO_FILE * __stream);

int main(void)
{
    struct _IO_FILE * fp;
    unsigned int wc;

    setlocale(6, "en_US.utf8");
    fp = fopen("fgetwc.dat", "w");
    if (!fp)
    {
        perror("Can't open file for writing");
        return 1;
    }
    fputs("\xd0\xba\xd0\xbe\xd1\x88\xd0\xba\xd0\xb0\n", fp);
    fclose(fp);
    fp = fopen("fgetwc.dat", "r");
    if (!fp)
    {
        perror("Can't open file for reading");
        return 1;
    }
    (*__errno_location()) = 0;
    while ((wc = fgetwc(fp)) != 4294967295)
    putwchar(wc);
    if (ferror(fp))
    {
        if ((*__errno_location()) == 84)
        {
            puts("Character encoding error while reading.");
        }
        else
        {
            puts("I/O error when reading");
        }
    }
    else
    {
        if (feof(fp))
        {
            puts("End of file reached successfully");
        }
    }
    fclose(fp);
}


