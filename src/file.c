#define TYPE(E, T) _Generic(typeof(E), T : 1, default: 0)

int main() {
  //static_assert(TYPE(0x80000000, unsigned int));
  static_assert(TYPE(2147483648, unsigned long));
}
