#pragma safety enable

_Clear void * _Owner calloc(unsigned long n, unsigned long s);
char * _Owner strdup(const char* s);
void free(void * _Nullable _Owner p);
void printf(char char* fmt, ...);

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
    char* _Owner name = user.name;
    free(name);
    set_id(&user, 1); //warning: object 'user.name' was moved  
}
