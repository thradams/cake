
struct X {
	union {
		int x;
	};
};

int main() {
	struct X x = (struct X){ .x = 50 };
}