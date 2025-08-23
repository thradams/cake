#define TEST
void main() {
   const char *str =
      "aaa "
#ifdef TEST
      "b\"b "
#else
      "ccc "
#endif
      "ddd ";
   str;
}