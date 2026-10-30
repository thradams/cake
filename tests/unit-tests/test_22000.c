#pragma safety enable

typedef int FILE;
FILE* _Owner fopen(const char* restrict filename, const char* restrict mode);
int fclose(FILE* _Owner stream);

int main()
{
   if (FILE* _Owner f = fopen("file.txt", "r"))
   {    
     /*...*/
     fclose(f);
   }
}

