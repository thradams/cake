struct X {
  union {
    struct {
      int *pSelect;
    } view2;
  }u;
};

int main()
{
    struct X t;
    t.u.view2.pSelect = 0;
}
