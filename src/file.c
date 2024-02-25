
int main() {
    int* ptr = 0;
    int k = *ptr;
    k = 0;
    return 0;
}

void dummy()
{
} 

#pragma cake diagnostic check "-Wanalyzer-null-dereference"

