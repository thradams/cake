#pragma safety enable

void * _Opt _Owner _Clear calloc(unsigned long n, unsigned long s);
char * _Opt _Owner strdup(const char* s);
void free(void * _Opt _Owner p);
void printf(char char* fmt, ...);

struct user
{
    int id;
    char* _Owner _Opt name;
};

void set_id(struct user* p, int id){}

int main()
{
    _Opt struct user user = {};
    user.name = strdup("a");
    char* _Owner _Opt name = user.name;
    free(name);
    set_id(&user, 1); //warning: object 'user.name' was moved  
}
