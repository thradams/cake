int woof();
int bark();

int use(int x) {
    defer{
      woof();
    }
    return bark();
}