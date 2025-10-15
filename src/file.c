struct index {
    int indx;
    char name[10];
    void* addr;
};

void f1(void)
{
}

void f2(void)
{
}


void main(void)
{
struct index my_index[] = {
  {1, "f1", f1},
  {2, "f2", f2},
  {0, "", 0 }
};
}