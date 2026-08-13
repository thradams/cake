#pragma safety enable


_Owner int socket();
void close(_Owner int fd);

int main()
{
  _Owner int fd; // owner object (fd) not moved (root see line 9)
  
  fd = socket();
  if (fd < 0)
  {     
     return 1; //lint 29 owner object (fd) not moved (rel-true see line 12)
  }
  close(fd); // passing a possible uninitialized object  'fd' (see line 9)
}
