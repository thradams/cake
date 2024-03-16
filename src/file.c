/*
  This is a work file
*/

int* _Owner f();
int main()
{    
    if (int* _Owner p = f()) 
    {
        static_debug(p);
    }

    if (int* _Owner p = f(); p == 0)
    {
        static_debug(p);
    }
    
    
}
