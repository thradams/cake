/* Cake 0.12.26 x86_msvc */

int socket();
void close(int fd);

int main()
{
    int fd;

    fd = socket();
    if (fd < 0)
    {
        return 1;
    }
    close(fd);
}


