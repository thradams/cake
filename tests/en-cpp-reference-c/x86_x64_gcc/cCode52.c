/* Cake x86_x64_gcc */
struct _IO_wide_data;

struct _IO_marker;
struct _IO_codecvt;

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

struct _IO_marker;

struct _IO_codecvt;


extern int puts(char * __s);

void show_orientation(int n)
{
    n < 0 ? puts("\tnarrow orientation") : n > 0 ? puts("\twide orientation") : puts("\tno orientation");
}


extern int fgetc(struct _IO_FILE * __stream);
extern int printf(char * __format, ...);
extern unsigned int fgetwc(struct _IO_FILE * __stream);

void try_read(struct _IO_FILE * fp)
{
    int c;
    unsigned int wc;

    c = fgetc(fp);
    if (c == -1)
    {
        puts("\tnarrow character read failed");
    }
    else
    {
        printf("\tnarrow character read '%c'\n", c);
    }
    wc = fgetwc(fp);
    if (wc == 4294967295)
    {
        puts("\twide character read failed");
    }
    else
    {
        printf("\twide character read '%lc'\n", wc);
    }
}


extern struct _IO_FILE *fopen(char * __filename, char * __modes);
extern void perror(char * __s);
extern int fwide(struct _IO_FILE * __fp, int __mode);
extern struct _IO_FILE *freopen(char * __filename, char * __modes, struct _IO_FILE * __stream);
extern int fclose(struct _IO_FILE * __stream);

int main(void)
{
    struct _IO_FILE * fp;

    fp = fopen("main.cpp", "r");
    if (!fp)
    {
        perror("fopen() failed");
        return 1;
    }
    puts("1) A newly opened stream has no orientation.");
    show_orientation(fwide(fp, 0));
    puts("2) Establish byte orientation.");
    show_orientation(fwide(fp, -1));
    try_read(fp);
    puts("3) Only freopen() can reset stream orientation.");
    if (freopen("main.cpp", "r", fp) == 0)
    {
        perror("freopen() failed");
        return 1;
    }
    puts("4) A reopened stream has no orientation.");
    show_orientation(fwide(fp, 0));
    puts("5) Establish wide orientation.");
    show_orientation(fwide(fp, 1));
    try_read(fp);
    fclose(fp);
}


