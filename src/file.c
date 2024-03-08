void * _Owner malloc(int i);
void free( void* _Owner _Opt p);
int rand();

int main() {
    char * _Owner s = malloc(1);
    //s : maybe-null
    try
    {
         static_debug(s);
         //s : maybe-null
         if (rand())
         {
             //s : maybe-null , maybe-null
             free(s);
             //s : maybe-null , *uninitialized
         }
         else
         {             
            static_debug(s);
            //throw;
         }
    }
    catch
    {
    }
    static_debug(s);
    static_state(s, "null or not-null or uninitialized");
}
