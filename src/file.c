int main(int argc, char *argv[]) {
  while (1) {
    if (argc) {
      goto one;
    }
    else {
      goto zero;
    }
    zero: 
       return 0;
    one: 
       return 1;
  }
}