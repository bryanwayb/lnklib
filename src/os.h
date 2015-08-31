#ifndef __OS_H__
#define __OS_H__

#undef OS_WINDOWS
#undef OS_UNIX
#undef OS_MAC_OSX
#undef OS_LINUX
#undef ARCH_X86
#undef ARCH_X64

// OS type

#if defined(__APPLE__) || defined(__MACH__)
#define OS_MAC_OSX
#endif

#if defined(__unix__) || defined(__unix)
#define OS_UNIX
#endif

#if defined(_WIN32) || defined(_WIN64)
#define OS_WINDOWS
#endif

#if defined(__linux__)
#define OS_LINUX
#endif

// Architectures

#if defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_IX86)
#define ARCH_X86
#endif

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
#define ARCH_X64
#endif

#endif