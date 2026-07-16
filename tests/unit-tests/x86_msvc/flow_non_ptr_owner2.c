/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

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
