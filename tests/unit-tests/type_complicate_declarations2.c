
void (*F(int a, int b))(void) { return 0; }

void (*(*PF)(int a, int b))(void) = F;

int main()
{
    PF(1, 2);
}
