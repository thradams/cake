
void F(int i);
_Owner int make();
int main()
{
    F(make());

#pragma cake diagnostic check "-Wnon-owner-move"
}
