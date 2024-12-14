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
     return 1;
  }
  close(fd);
}

