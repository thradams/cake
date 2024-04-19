void * /*_Owner*/ calloc(int i, int sz);
void free( void * /*_Owner*/ p);

struct X { int i; };
struct Y { struct X * /*_Owner*/ p; };

int main() {
   struct Y y = {0};
   struct X * /*_Owner*/ p = calloc(1, sizeof(struct X));
   if (p){
     y.p = p;
   }
  free(y.p);
}

