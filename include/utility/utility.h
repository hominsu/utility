//
// Created by Homing So on 2023/8/7.
//

#ifndef UTILITY_INCLUDE_UTILITY_UTILITY_H_
#define UTILITY_INCLUDE_UTILITY_UTILITY_H_

#ifndef UTILITY_ASSERT
#include <cassert>
#define UTILITY_ASSERT(x) assert(x)
#endif // UTILITY_ASSERT

#ifndef UTILITY_LENGTH
#define UTILITY_LENGTH(CONST_ARRAY) sizeof(CONST_ARRAY) / sizeof(CONST_ARRAY[0])
#endif // UTILITY_LENGTH

#ifndef UTILITY_STR_LENGTH
#if defined(_MSC_VER)
#define UTILITY_STR_LENGTH(CONST_STR) _countof(CONST_STR)
#else
#define UTILITY_STR_LENGTH(CONST_STR) sizeof(CONST_STR) / sizeof(CONST_STR[0])
#endif
#endif // UTILITY_STR_LENGTH

#if defined(_WIN64) || defined(WIN64) || defined(_WIN32) || defined(WIN32)
#if defined(_WIN64) || defined(WIN64)
#define UTILITY_ARCH_64 1
#else
#define UTILITY_ARCH_32 1
#endif
#define UTILITY_PLATFORM_STRING "windows"
#define UTILITY_WINDOWS 1
#elif defined(__linux__)
#define UTILITY_PLATFORM_STRING "linux"
#define UTILITY_LINUX 1
#ifdef _LP64
#define UTILITY_ARCH_64 1
#else /* _LP64 */
#define UTILITY_ARCH_32 1
#endif /* _LP64 */
#elif defined(__APPLE__)
#define UTILITY_PLATFORM_STRING "osx"
#define UTILITY_APPLE 1
#ifdef _LP64
#define UTILITY_ARCH_64 1
#else /* _LP64 */
#define UTILITY_ARCH_32 1
#endif /* _LP64 */
#endif

#ifndef UTILITY_WINDOWS
#define UTILITY_WINDOWS 0
#endif
#ifndef UTILITY_LINUX
#define UTILITY_LINUX 0
#endif
#ifndef UTILITY_APPLE
#define UTILITY_APPLE 0
#endif

#endif //UTILITY_INCLUDE_UTILITY_UTILITY_H_
