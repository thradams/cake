
#pragma nullable enable
#pragma ownership enable

void * /*_Owner*/ /*_Opt*/ malloc(int i);
void free( void * /*_Owner*/ /*_Opt*/  p);

struct X {
  char * /*_Owner*/ name;    
};

int main() {
   struct X * /*_Owner*/ /*_Opt*/ p = malloc(sizeof * p);
   free(p);   
} 
