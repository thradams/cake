/*
  C2Y 6.8.5.1 selection-header (N3580)
  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3580.htm

  The controlling clause of if and switch accepts three forms:

    expression
    declaration expression
    declaration-condition    (T D = X  is treated as  T D = X; D)

  The declared object is visible in the secondary block and in the else
  branch, and goes out of scope after the statement.
*/

#include <stdio.h>

int main()
{
   /* expression */
   FILE* f0;
   if ( f0 = fopen("file.txt", "r"))
   {
     /*...*/
     fclose(f0);
   }

   /* declaration expression */
   if (FILE* f = fopen("file.txt", "r"); f != NULL)
   {
     /*...*/
     fclose(f);
   }

   /* declaration-condition: f itself is the controlling expression */
   if (FILE* f = fopen("file.txt", "r"))
   {
     /*...*/
     fclose(f);
   }
   else
   {
     /* f is visible here too */
     printf("f is %p\n", (void*)f);
   }

   /* the same forms work with switch */
   switch (int c = getchar())
   {
     case EOF: break;
     default: putchar(c); break;
   }

   switch (int c = getchar(); c + 1)
   {
     case 0: break;
     default: putchar(c); break;
   }

   /* c is out of scope here */
   int c = 0;
   return c;
}
