#pragma safety enable


char * /*_Owner*/ strdup(const char* s);
void free(void * /*_Owner*/ p);

struct X {
  char */*_Owner*/ /*_Opt*/ name;
};

struct X make()
{
  struct X x = {0};
  x.name = strdup("text");  
  return x;
}
