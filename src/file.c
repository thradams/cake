
#pragma safety enable

char* _Owner _Opt strdup(const char*);
void free(void* _Owner _Opt ptr);

void f(const char* _Opt s) {
   const char* _Owner _Opt p;
   p  = s ? strdup(s) : 0;
   free((void* _Owner)p);
}

