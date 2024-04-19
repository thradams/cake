
void* /*_Owner*/ malloc(unsigned size);
void free(void* /*_Owner*/ ptr);

int main() {
   void * /*_Owner*/ p = malloc(1);
   do{
      free(p);
   }
   while(0);   
}
