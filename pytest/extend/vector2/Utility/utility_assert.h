#ifndef __UTILITY_ASSERT_H__
#define __UTILITY_ASSERT_H__

#include <assert.h>

#ifdef _DEBUG
#   define UTILITY_ASSERT(expr) assert(expr)
#else
#   define UTILITY_ASSERT(expr)
#endif

#endif // __UTILITY_ASSERT_H__