#pragma safety enable


void * _Owner malloc(int i);
void free( void * _Owner _Opt p);

struct X {
  char * _Owner name;    
};

int main() {
   struct X * _Owner p = malloc(sizeof * p);
   if (p) {
     p->name = malloc(10);
     free(p->name);
   }
   free(p);   
} 
