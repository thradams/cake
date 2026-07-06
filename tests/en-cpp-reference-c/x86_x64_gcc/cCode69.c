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


extern unsigned int short **__ctype_b_loc(void);
extern int getc(struct _IO_FILE * __stream);
extern int printf(char * __format, ...);
extern int ungetc(int __c, struct _IO_FILE * __stream);

void demo_scanf(char * fmt, struct _IO_FILE * s)
{
    while (*fmt != 0)
    {
        if (*fmt == 37)
        {
            int c;

            /*switch*/
            {
                char __v0 = *++fmt;
                if (__v0 == 117) goto __L2; /*case 117*/
                if (__v0 == 99) goto __L3; /*case 99*/
                goto __L1;

                {
                    unsigned int num;

                    __L2: /*case 117*/ 
                    while (((*__ctype_b_loc())[(int)(c = getc(s))] & 8192))
                    {
                    }
                    num = 0;
                    while (((*__ctype_b_loc())[(int)(c)] & 2048))
                    {
                        num = num * 10 + c - 48;
                        c = getc(s);
                    }
                    printf("%%u scanned %u\n", num);
                    ungetc(c, s);
                    goto __L1; /* break */

                    __L3: /*case 99*/ 
                    c = getc(s);
                    printf("%%c scanned '%c'\n", c);
                    goto __L1; /* break */

                }
                __L1:;
            }
        }
        else
        {
            ++fmt;
        }
    }
}


extern struct _IO_FILE *fopen(char * __filename, char * __modes);
extern int fputs(char * __s, struct _IO_FILE * __stream);
extern void rewind(struct _IO_FILE * __stream);
extern int fclose(struct _IO_FILE * __stream);

int main(void)
{
    struct _IO_FILE * f;

    f = fopen("input.txt", "w+");
    if (f != 0)
    {
        fputs("123x", f);
        rewind(f);
        demo_scanf("%u%c", f);
        fclose(f);
    }
    return 0;
}


