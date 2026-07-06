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
extern int fprintf(struct _IO_FILE * __stream, char * __format, ...);
extern struct _IO_FILE * stderr;
extern void abort(void);
extern int fclose(struct _IO_FILE * __stream);
extern int printf(char * __format, ...);

int main(void)
{
    struct _IO_FILE * fp;

    fp = fopen("data.txt", "r");
    if (fp == 0)
    {
        fprintf(stderr, "error opening file data.txt in function main()\n");
        abort();
    }
    fclose(fp);
    printf("Normal Return\n");
    return 0;
}


