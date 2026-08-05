#pragma safety enable

/*
   A global (file-scope) pointer respects its declared nullability, just like a
   parameter or a member:
     - a non-_Opt global pointer is non-null;
     - an _Opt global pointer is possibly-null.

   flow3 used to seed every global read as a generic ANY value, so a non-_Opt
   global pointer read as possibly-null -- e.g. passing the standard `stdout`
   (declared non-_Opt) to a non-nullable parameter falsely warned.
*/

#define NULL ((void*)0)

extern int* non_opt_global;          /* non-_Opt -> non-null */
extern int* _Opt opt_global;         /* _Opt     -> possibly null */

void takes_nonnull(int* p);

/* Non-_Opt global is non-null: no warning. */
void use_non_opt(void)
{
    takes_nonnull(non_opt_global);
    *non_opt_global = 1;
}

/* _Opt global is possibly-null: must be guarded. */
void use_opt(void)
{
    if (opt_global != NULL)
        takes_nonnull(opt_global);   /* ok: guarded */
}

/*
   The original case -- passing the non-null standard stream is clean:

       #include <stdio.h>
       void log_line(const char* s) { fputs(s, stdout); }   // stdout is non-null
*/
