struct _iobuf {
    void * _Placeholder;
};


struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
void __cdecl perror(char * _ErrorMessage);
int __cdecl puts(char * _Buffer);
int __cdecl fclose(struct _iobuf * _Stream);
int __cdecl remove(char * _FileName);

int main(void)
{
    struct _iobuf * fp;
    int rc;

    fp = fopen("file1.txt", "w");
    if (!fp)
    {
        perror("file1.txt");
        return 1;
    }
    puts("Created file1.txt");
    fclose(fp);
    rc = remove("file1.txt");
    if (rc)
    {
        perror("remove");
        return 1;
    }
    puts("Removed file1.txt");
    fp = fopen("file1.txt", "r");
    if (!fp)
    {
        perror("Opening removed file failed");
    }
    rc = remove("file1.txt");
    if (rc)
    {
        perror("Double-remove failed");
    }
    return 0;
}

