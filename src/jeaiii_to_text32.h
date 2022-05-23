/*
MIT License

Copyright (c) 2022 James Edward Anhalt III - https://github.com/jeaiii/itoa

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#if defined(__clang__) || (defined(__GNUC__) && defined(__llvm__))
#define JEAIII_CLANG 1
#elif defined(_MSC_VER)
#pragma warning(disable : 4820)
#pragma warning(disable : 4371)
#define JEAIII_MSVC 1
#elif defined(__GNUC__) && !defined(__clang__)
#define JEAIII_GNUCXX 1
#endif

#if defined(JEAIII_MSVC)
#define JEAIII_VISUAL_STUDIO 1
#if defined(JEAIII_CLANG)
#define JEAIII_CLANG_VISUAL_STUDIO 1
#else
#define JEAIII_REGULAR_VISUAL_STUDIO 1
#endif
#endif

#if (defined(__x86_64__) || defined(_M_AMD64)) && !defined(_M_ARM64EC)
#define JEAIII_IS_X86_64 1
#else
#define JEAIII_IS_ARM64 1
#endif

#define JEAIII_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)

#if defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__)) || defined(TARGET_OS_MAC)
#define JEAIII_MAC 1
#define RTC_MAC
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
#define JEAIII_LINUX 1
#define RTC_LZCNT
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#define JEAIII_WIN 1
#define RTC_LZCNT
#else
#error "Undetected platform."
#endif

#if defined(__has_builtin)
#define JEAIII_HAS_BUILTIN(x) __has_builtin(x)
#else
#define JEAIII_HAS_BUILTIN(x) 0
#endif

#if !defined(JEAIII_LIKELY)
#define JEAIII_LIKELY(...) (__VA_ARGS__) [[likely]]
#endif

#if !defined(JEAIII_UNLIKELY)
#define JEAIII_UNLIKELY(...) (__VA_ARGS__) [[unlikely]]
#endif

#if !defined(JEAIII_ELSE_UNLIKELY)
#define JEAIII_ELSE_UNLIKELY(...) __VA_ARGS__ [[unlikely]]
#endif

#if defined(JEAIII_GNUCXX) || defined(JEAIII_CLANG)
#define JEAIII_ASSUME(x) \
		do { \
			if (!(x)) \
				__builtin_unreachable(); \
		} while (0)
#elif defined(JEAIII_MSVC)
#include <intrin.h>
#define JEAIII_ASSUME(x) __assume(x)
#else
#define JEAIII_ASSUME(x) (( void )0)
#endif

#if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L
#define JEAIII_HAS_INLINE_VARIABLE 1
#elif __cplusplus >= 201703L
#define JEAIII_HAS_INLINE_VARIABLE 1
#elif defined(JEAIII_MSVC) && JEAIII_MSVC >= 1912 && _MSVC_LANG >= 201703L
#define JEAIII_HAS_INLINE_VARIABLE 1
#else
#define JEAIII_HAS_INLINE_VARIABLE 0
#endif

#if JEAIII_HAS_INLINE_VARIABLE
#define JEAIII_INLINE_VARIABLE inline constexpr
#else
#define JEAIII_INLINE_VARIABLE static constexpr
#endif

#if defined(JEAIII_MSVC)
#define JEAIII_INLINE [[msvc::forceinline]] inline
#define JEAIII_NON_GCC_INLINE [[msvc::forceinline]] inline
#define JEAIII_CLANG_MACOS_INLINE
#define JEAIII_CLANG_INLINE
#elif defined(JEAIII_CLANG)
#if defined(JEAIII_MAC)
#define JEAIII_CLANG_MACOS_INLINE inline __attribute__((always_inline))
#else
#define JEAIII_CLANG_MACOS_INLINE
#endif
#define JEAIII_NON_GCC_INLINE inline __attribute__((always_inline))
#define JEAIII_NO_INLINE __attribute__((noinline))
#define JEAIII_INLINE inline __attribute__((always_inline))
#define JEAIII_CLANG_INLINE inline __attribute__((always_inline))
#elif defined(JEAIII_GNUCXX)
#define JEAIII_INLINE inline __attribute__((always_inline))
#define JEAIII_NON_GCC_INLINE
#define JEAIII_CLANG_INLINE
#define JEAIII_CLANG_MACOS_INLINE
#endif

namespace jeaiii32
{
    using u32 = decltype(0xffffffff);
    using u64 = decltype(0xffffffffffffffff);
    using size_t = decltype(sizeof(0));

    struct pair { char dd[2]; };

    static constexpr union
    {
        char _[402]{
            "00010203040506070809"
            "10111213141516171819"
            "20212223242526272829"
            "30313233343536373839"
            "40414243444546474849"
            "50515253545556575859"
            "60616263646566676869"
            "70717273747576777879"
            "80818283848586878889"
            "90919293949596979899"

            "0\0001\0002\0003\0004\0005\0006\0007\0008\0009\000"
            "10111213141516171819"
            "20212223242526272829"
            "30313233343536373839"
            "40414243444546474849"
            "50515253545556575859"
            "60616263646566676869"
            "70717273747576777879"
            "80818283848586878889"
            "90919293949596979899"

            "\000"
        };
        struct
        {
            pair dd[100];
            pair fd[101];
        };
    }
    digits{ };

        //https://godbolt.org/z/b5qe1bTev

    JEAIII_INLINE char* to_text_n32(char* b, u32 n) noexcept
    {
        if (n < u32(1e2))
        {
            *(pair*)b = digits.fd[n];
            return n < 10 ? b + 1 : b + 2;
        }
        if (n < u32(1e6))
        {
            if (n < u32(1e4))
            {
                auto f0 = u32(10 * 0x1p24 / 1e3 + 1) * n;
                *reinterpret_cast<pair*>(b) = digits.fd[f0 >> 24];
                b -= n < u32(1e3);
                auto f2 = (f0 & u32(0x1p24) - 1) * 100;
                *reinterpret_cast<pair*>(b + 2) = digits.dd[f2 >> 24];
                return b + 4;
            }
            auto f0 = u64(10 * 0x1p32 / 1e5 + 1) * n;
            *reinterpret_cast<pair*>(b) = digits.fd[f0 >> 32];
            b -= n < u32(1e5);
            auto f2 = (f0 & u64(0x1p32) - 1) * 100;
            *reinterpret_cast<pair*>(b + 2) = digits.dd[f2 >> 32];
            auto f4 = (f2 & u64(0x1p32) - 1) * 100;
            *reinterpret_cast<pair*>(b + 4) = digits.dd[f4 >> 32];
            return b + 6;
        }
        if (n < u32(1e8))
        {
            auto f0 = u64(10 * 0x1p48 / 1e7 + 1) * n >> 16;
            *reinterpret_cast<pair*>(b) = digits.fd[f0 >> 32];
            b -= n < u32(1e7);
            auto f2 = (f0 & u64(0x1p32) - 1) * 100;
            *reinterpret_cast<pair*>(b + 2) = digits.dd[f2 >> 32];
            auto f4 = (f2 & u64(0x1p32) - 1) * 100;
            *reinterpret_cast<pair*>(b + 4) = digits.dd[f4 >> 32];
            auto f6 = (f4 & u64(0x1p32) - 1) * 100;
            *reinterpret_cast<pair*>(b + 6) = digits.dd[f6 >> 32];
            return b + 8;
        }
        auto f0 = u64(10 * 0x1p58 / 1e9 + 1) * n >> 26;
        *reinterpret_cast<pair*>(b) = digits.fd[f0 >> 32];
        b -= n < u32(1e9);
        auto f2 = (f0 & u64(0x1p32) - 1) * 100;
        *reinterpret_cast<pair*>(b + 2) = digits.dd[f2 >> 32];
        auto f4 = (f2 & u64(0x1p32) - 1) * 100;
        *reinterpret_cast<pair*>(b + 4) = digits.dd[f4 >> 32];
        auto f6 = (f4 & u64(0x1p32) - 1) * 100;
        *reinterpret_cast<pair*>(b + 6) = digits.dd[f6 >> 32];
        auto f8 = (f6 & u64(0x1p32) - 1) * 100;
        *reinterpret_cast<pair*>(b + 8) = digits.dd[f8 >> 32];
        return b + 10;
    };

    JEAIII_INLINE char* to_text_n64(char* b, u64 n) noexcept
    {
        if (n == u32(n))
            return to_text_n32(b, u32(n));

        u64 h = n / u32(1e8);
        u32 z = n % u32(1e8);

        if (h == u32(h))
        {
            b = to_text_n32(b, u32(h));
        }
        else
        {
            u32 x = h / u32(1e8);
            u32 y = h % u32(1e8);

            // x is 2, 3, or 4 digits
            if (x < 100)
            {
                *(pair*)b = digits.dd[x];
                b += 2;
            }
            else
            {
                auto t = (u64(0x1p32) / u64(1e2) + 1) * x;
                *(pair*)b = digits.fd[t >> 32], b -= x < 1000;
                t = u32(t), t *= 100, * (pair*)&b[2] = digits.dd[t >> 32];
                b += 4;
            }
            auto t = (u64(0x1p48) / u64(1e6) + 1) * y >> 16;
            *(pair*)b = digits.dd[t >> 32];
            t = u32(t), t *= 100, * (pair*)&b[2] = digits.dd[t >> 32];
            t = u32(t), t *= 100, * (pair*)&b[4] = digits.dd[t >> 32];
            t = u32(t), t *= 100, * (pair*)&b[6] = digits.dd[t >> 32];
            b += 8;
        }
        auto t = (u64(0x1p48) / u64(1e6) + 1) * z >> 16;
        *(pair*)b = digits.dd[t >> 32];
        t = u32(t), t *= 100, * (pair*)&b[2] = digits.dd[t >> 32];
        t = u32(t), t *= 100, * (pair*)&b[4] = digits.dd[t >> 32];
        t = u32(t), t *= 100, * (pair*)&b[6] = digits.dd[t >> 32];
        return b + 8;
    }






}
