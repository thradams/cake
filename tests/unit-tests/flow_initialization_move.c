#pragma safety enable;

struct X
 {
   char * _Owner _Opt p;
 };

 char* _Owner _Opt strdup(const char *s);
 void free(void* _Owner _Opt p);

 int main() {
     const char* _Owner _Opt p0 = strdup("a");
     struct X x = {
         .p = p0
     };         
     free(x.p);
 }
