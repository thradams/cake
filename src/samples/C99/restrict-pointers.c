   void copy_array(int n, int *restrict a, int *restrict b)
   {
      while (n-- > 0)
        *a++ = *b++;
   }
