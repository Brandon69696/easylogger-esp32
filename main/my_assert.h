#pragma once
#include "elog_exception.h"

#define MY_ASSERT(expr) do { \
    if (!(expr)) { \
        elog_user_assert_hook(#expr, __FUNCTION__, __LINE__); \
    } \
} while (0)
