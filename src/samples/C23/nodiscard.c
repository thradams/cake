
#include <stdlib.h>

struct [[nodiscard]] error_info { int error; };

struct error_info enable_missile_safety_mode(void);

void launch_missiles(void);

void test_missiles(void) {
    enable_missile_safety_mode();
    launch_missiles();
}

[[nodiscard("must check armed state")]]
bool arm_detonator(int within);

void detonate();

void call(void) {
  arm_detonator(3);
  detonate();
}
