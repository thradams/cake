
#pragma safety enable

void free(void* _Owner _Nullable ptr);
char* _Owner _Nullable strdup(char const* _String);

struct user
{
  int id;
  char* _Owner name;
};

void set_id(struct user* p, int id){}

int main()
{
  _Nullable struct user user = {};
  user.name = strdup("a");
  char* _Owner _Nullable name = user.name;
  free(name);
  static_debug(user.name);
  if (user.name)
    set_id(&user, 1); //warning: object 'user.name' was moved  
}
