#pragma safety enable

void free(void *_Owner _Opt p);
char * _Owner strdup(const char* s);

struct X {
  char *_Owner text;
};

struct X * _Nonnull _Owner make();

void f(int condition)
{
    struct X * _Owner p = nullptr;
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
