int main() {
	int a[5] = {0};
	int i = a[5];
#pragma cake diagnostic check "-Wout-of-bounds"
}
