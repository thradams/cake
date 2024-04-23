#pragma ownership enable
#pragma nullable enable

void free(void* _Owner _Opt ptr);
char* _Owner _Opt strdup(char const* _String);

struct user
{
  int id;
  char* _Owner name;
};

void set_id(struct user* p, int id){}

int main()
{
  _Opt struct user user = {};
  user.name = strdup("a");
  char* _Owner _Opt name = user.name; //we need to create an object when we move
  static_debug(user);  
  free(name);
  static_debug(user.name);
  if (user.name)
    set_id(&user, 1); //warning: object 'user.name' was moved  
}
