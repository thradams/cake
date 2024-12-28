// a.c
struct X {
	union {
		int x;
		int a;
	};
	union {
		int y;
	};
};

int main() {
	constexpr struct X x = { .y = 50 };
	static_assert(x.y == 50);
}