
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

