/*
  This is a work file
*/

int* f();
int main()
{    
    if (int* p = f()) 
    {
        static_debug(p);
    }
}
