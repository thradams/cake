
#pragma nullable enable
#pragma ownership enable

char * _Owner _Opt strdup(const char* s);
void free(void * _Opt _Owner p);

struct X {
  char *_Owner name;
};

void x_destroy(_Opt struct X x) 
{
  free(x.name);
}

int main() {
   _Opt struct X x = {0};
   x.name = strdup("a");
   x_destroy(x);
}


