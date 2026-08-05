#pragma safety enable;

struct X
 {
   char * _Owner _Opt p;
   char * p2;
 };

 char* _Owner _Opt strdup(const char *s);
 void free(void* _Owner _Opt p);

 int main() {
     const char* _Owner _Opt p0 = strdup("a");
     struct X x = {
         .p = p0                // moves p0 into the owner member x.p
     };
     free(x.p); // ok: x.p owns the string (moved from p0); freeing it is valid
 }
