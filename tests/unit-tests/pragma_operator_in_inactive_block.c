/*
   _Pragma inside an inactive block used to crash the preprocessor.

   prematch() DELETES the token instead of appending it when the block is
   inactive (unless -keep-inactive-tokens), so operator_pragma dereferenced a
   null r.tail; guarding that exposed a second bug -- the pragma's own tokens
   were still emitted into an inactive region, so the parser reported
   "expected declaration not 'once'" and printed the location from a
   token_origin that had already been freed.
*/

#if 0
_Pragma("once")
#endif

#if 0
_Pragma("GCC diagnostic push")
_Pragma("GCC diagnostic ignored \"-Wunused\"")
#endif

/* still works where it is active */
_Pragma("once")

int main()
{
    return 0;
}
