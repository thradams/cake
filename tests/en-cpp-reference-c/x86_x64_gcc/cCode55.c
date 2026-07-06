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


extern int swscanf(int * __s, int * __format, ...);
extern int fprintf(struct _IO_FILE * __stream, char * __format, ...);
extern struct _IO_FILE * stderr;
extern int wprintf(int * __format, ...);
extern struct _IO_FILE *tmpfile(void);
extern int fwprintf(struct _IO_FILE * __stream, int * __format, ...);
extern int fclose(struct _IO_FILE * __stream);
extern void rewind(struct _IO_FILE * __stream);
extern int fwscanf(struct _IO_FILE * __stream, int * __format, ...);

int main(void)
{
    int state[64];
    int capital[64];
    unsigned int population;
    int elevation;
    int age;
    float pi;
    int * input;
    struct _IO_FILE * fp;

    population = 0;
    elevation = 0;
    age = 0;
    pi = 0;
    input = L"California 170 3.141592";
    if (swscanf(input, L"%ls%d%f", state, &age, &pi) != 3)
    {
        fprintf(stderr, "Error reading input.\n");
        return 2;
    }
    wprintf(L"State: %ls\nAge  : %d years\nPi   : %.5f\n\n", state, age, pi);
    fp = tmpfile();
    if (fp)
    {
        if (!fwprintf(fp, L"Mississippi Jackson 420000 807"))
        {
            fprintf(stderr, "Error writing to file.\n");
            fclose(fp);
            return 3;
        }
        rewind(fp);
        fwscanf(fp, L"%ls%ls%u%d", state, capital, &population, &elevation);
        wprintf(L"State  : %ls\nCapital: %ls\nJackson population (in 2020): %u\nHighest elevation: %dft\n", state, capital, population, elevation);
        fclose(fp);
    }
}


