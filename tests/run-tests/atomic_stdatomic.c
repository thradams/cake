#include <stdio.h>
#include <stdatomic.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

atomic_int counter;
atomic_flag flag = ATOMIC_FLAG_INIT;
_Atomic(int*) ap;
int arr[4];

int main(void)
{
    atomic_int x;
    atomic_init(&x, 10);
    CHECK(atomic_load(&x) == 10);
    atomic_store(&x, 20);
    CHECK(atomic_load_explicit(&x, memory_order_acquire) == 20);
    CHECK(atomic_exchange(&x, 30) == 20);

    int expected = 30;
    CHECK(atomic_compare_exchange_strong(&x, &expected, 40));
    expected = 0;
    CHECK(!atomic_compare_exchange_weak(&x, &expected, 50));
    CHECK(expected == 40);

    CHECK(atomic_fetch_add(&x, 2) == 40);
    CHECK(atomic_fetch_sub_explicit(&x, 1, memory_order_relaxed) == 42);
    CHECK(atomic_fetch_or(&x, 4) == 41);
    CHECK(atomic_fetch_and(&x, 7) == 45);
    CHECK(atomic_fetch_xor(&x, 1) == 5);
    CHECK(x == 4);

    CHECK(!atomic_flag_test_and_set(&flag));
    CHECK(atomic_flag_test_and_set(&flag));
    atomic_flag_clear(&flag);
    CHECK(!atomic_flag_test_and_set(&flag));

    atomic_store(&ap, arr);
    atomic_fetch_add(&ap, 2);
    CHECK(atomic_load(&ap) == arr + 2);

    atomic_thread_fence(memory_order_seq_cst);
    CHECK(atomic_is_lock_free(&counter));

    counter++;
    counter += 2;
    CHECK(atomic_load(&counter) == 3);

    return failures;
}
