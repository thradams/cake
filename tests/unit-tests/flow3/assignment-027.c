struct X { int i; void* _Nonnull p; };
int main() {
	struct X x;
	x = (struct X){ 0 };
	compile_assert(x.i == 0);
	compile_assert(x.p == 0);
}
