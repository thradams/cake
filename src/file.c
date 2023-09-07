char * owner strdup(const char* s);
void free(void char * owner p);

struct X {
  char *owner name;
};

void f(char * owner s);

void x_destroy(struct X * obj_owner p) 
{
  free(p->name);
}

int main() {
   struct X x = {0};
   x.name = strdup("a");
   f(x.name);
   static_debug(x);
   x_destroy(&x);
}
