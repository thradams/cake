
void * owner malloc(int i);

struct X {
  void * owner text;
};

void x_change( struct X* list);

int main()
{
  struct X * owner x = malloc(sizeof * x);
  static_debug(x);
  x_change(x);
}
