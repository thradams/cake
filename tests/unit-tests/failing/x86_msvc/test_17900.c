/* Cake x86_msvc */
struct user {
    int id;
    char * name;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void set_id(struct user * p, int id)
{
}


char *strdup(char * _String);
void free(void * ptr);

int main()
{
    struct user  user;
    char * name;

    _cake_zmem(&user, 8);
    user.name = strdup("a");
    name = user.name;
    free(name);
    if (user.name)
    {
        set_id(&user, 1);
    }
}


