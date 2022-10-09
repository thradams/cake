
#pragma once

enum {
    UNINITIALIZED = 1 << 1,
    MUST_DESTROY = 1 << 2,
    MUST_FREE = 1 << 3,
    MOVED = 1 << 4
};

