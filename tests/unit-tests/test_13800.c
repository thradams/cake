struct X { int i; void* p; };
int main() {
	struct X x;
	x = (struct X){ 0 };
	static_state(x.i, "zero");
	static_state(x.p, "null");
}