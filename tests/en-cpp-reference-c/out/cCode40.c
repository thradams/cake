struct _iobuf {
    void * _Placeholder;
};


struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
void __cdecl perror(char * _ErrorMessage);
int __cdecl fputs(char * _Buffer, struct _iobuf * _Stream);
void __cdecl rewind(struct _iobuf * _Stream);
int __cdecl fgetc(struct _iobuf * _Stream);
int __cdecl putchar(int _Character);
int __cdecl ferror(struct _iobuf * _Stream);
int __cdecl puts(char * _Buffer);
int __cdecl feof(struct _iobuf * _Stream);
int __cdecl fclose(struct _iobuf * _Stream);
int __cdecl remove(char * _FileName);

int main(void)
{
    char * fname;
    int is_ok;
    struct _iobuf * fp;
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

