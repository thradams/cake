void * /*_Owner*/ malloc(int i);
void free( void * /*_Owner*/ p);

struct X {
  char * /*_Owner*/ name;    
};

int main() {
   struct X * /*_Owner*/ p = malloc(sizeof * p);
   free(p);   
} 
