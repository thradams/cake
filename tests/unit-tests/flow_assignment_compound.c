struct X { int i; void* p; };
int main() {
	struct X x;
	x = (struct X){ 0 };
	assert_state(x.i, "zero");
	assert_state(x.p, "null");
}