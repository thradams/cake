typedef struct index {
    int indx;
    char name[10];
    void *addr;
} index_t;

extern void f1(void);

extern void f2(void);

index_t my_index[] = { 
  {1, "f1", f1}, 
  {2, "f2", f2}, 
  {0, "", 0 }
};

void main(void) {
}