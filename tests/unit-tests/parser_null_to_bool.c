void f(bool b);

int main(void)
{
    bool b = nullptr;
#pragma cake diagnostic check "-Wnull-conversion"

    b = nullptr;
#pragma cake diagnostic check "-Wnull-conversion"
    f(nullptr);
#pragma cake diagnostic check "-Wnull-conversion"
}
