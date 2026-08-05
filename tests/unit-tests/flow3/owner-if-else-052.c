#pragma safety enable

void free(void *_Owner _Opt p);
char * _Owner _Opt strdup(const char* s);

struct X {
  char *_Owner _Opt text;
};

struct X * _Owner make();

void f(int condition)
{
    struct X * _Owner _Opt p = nullptr;
    if (condition)
    {
        p = make();        
    }
    else
    {
        p = make();        
    }
    
    free(p->text);
    p->text = strdup("c");
    
    free(p->text);
    free(p);
}
