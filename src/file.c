struct X {
    union {
        struct {
            int Zone : 28;
            int Level : 4;
        };
        int Value;
    };
};
int main()
{
  int i = sizeof(struct X);
}
