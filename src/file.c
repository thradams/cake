void f(char* s, ...);
int main() {
    int i;
    f("", i);
//first pass analyze
#pragma cake diagnostic check "-uninitialized"
    return 0;
}
void dummy()
{
} 
//flow analyze
#pragma cake diagnostic check "-Wmaybe-uninitialized"

