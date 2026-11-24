#pragma safety enable

char* _Owner strdup(const char*);
void free(void* _Owner _Nullable ptr);

void f(const char* _Nullable s) {
   const char* _Owner p;
   
   /* The resulting expression must be char * _Nullable _Owner */
   p  = s ? strdup(s) : 0;

   free((void*)p);
}
