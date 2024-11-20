#pragma safety enable


_Owner int socket();
void close(_Owner int fd);

int main()
{
  _Owner int fd;

  fd = socket();
  if (fd < 0)
  {
     static_set(fd, "null");
     static_debug(fd);
     return 1;
  }
  close(fd);
}
