#pragma safety enable


void* /*_Owner*/ malloc(unsigned long size);
void free(void* /*_Owner*/ /*_Opt*/ ptr);

struct Y {
  char * /*_Owner*/ p0;
  int * /*_Owner*/ p2;
  double i2;
};

struct X {
  char * /*_Owner*/ text;
  int * /*_Owner*/ p1;
  int i;
  struct Y  *pY;
};

int main() {   
   struct X * /*_Owner*/ x = malloc(sizeof * x);
   

   
   
   
   free(x);
}

