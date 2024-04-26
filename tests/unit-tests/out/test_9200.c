#pragma nullable enable
#pragma ownership enable


void* /*_Owner*/ /*_Opt*/ malloc(unsigned size);
void free(void* /*_Owner*/ /*_Opt*/  ptr);

int main() {
   void * /*_Owner*/ /*_Opt*/ p = malloc(1);
   do{
      free(p);
   }
   while(0);   
}
