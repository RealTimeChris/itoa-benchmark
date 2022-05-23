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

namespace jeaiii
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

    template<bool, class, class F> struct _cond { using type = F; };
    template<class T, class F> struct _cond<true, T, F> { using type = T; };
    template<bool B, class T, class F> using cond = typename _cond<B, T, F>::type;

    template<class T> JEAIII_INLINE 
    char* to_text_from_integer(char* b, T i)
    {
        constexpr auto q = sizeof(T);
        using U = cond<q == 1, unsigned char, cond<q <= sizeof(short), unsigned short, cond<q <= sizeof(u32), u32, u64>>>;

        U n = i < 0 ? *b++ = '-', U(0) - U(i) : U(i);

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
        if (n < u64(0x1p32))
        {
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

            auto f0 = u64(10 * u64(0x1p57) / u64(1e9) + 1) * u32(n);
            *reinterpret_cast<pair*>(b) = digits.fd[f0 >> 57];
            b -= n < u32(1e9);
            auto f2 = (f0 & u64(0x1p57) - 1) * 100;
            *reinterpret_cast<pair*>(b + 2) = digits.dd[f2 >> 57];
            auto f4 = (f2 & u64(0x1p57) - 1) * 100;
            *reinterpret_cast<pair*>(b + 4) = digits.dd[f4 >> 57];
            auto f6 = (f4 & u64(0x1p57) - 1) * 100;
            *reinterpret_cast<pair*>(b + 6) = digits.dd[f6 >> 57];
            auto f8 = (f6 & u64(0x1p57) - 1) * 100;
            *reinterpret_cast<pair*>(b + 8) = digits.dd[f8 >> 57];
            return b + 10;
        }

        u32 z = n % u32(1e8);
        n /= u32(1e8);

        if (n < u32(1e2))
        {
            *(pair*)b = digits.fd[n];
            b = n < 10 ? b + 1 : b + 2;
        }
        else if (n < u32(1e6))
        {
            if (n < u32(1e4))
            {
                auto f0 = u32(10 * 0x1p24 / 1e3 + 1) * n;
                *reinterpret_cast<pair*>(b) = digits.fd[f0 >> 24];
                b -= n < u32(1e3);
                auto f2 = (f0 & u32(0x1p24) - 1) * 100;
                *reinterpret_cast<pair*>(b + 2) = digits.dd[f2 >> 24];
                b += 4;
            }
            else
            {
                auto f0 = u64(10 * 0x1p32 / 1e5 + 1) * n;
                *reinterpret_cast<pair*>(b) = digits.fd[f0 >> 32];
                b -= n < u32(1e5);
                auto f2 = (f0 & u64(0x1p32) - 1) * 100;
                *reinterpret_cast<pair*>(b + 2) = digits.dd[f2 >> 32];
                auto f4 = (f2 & u64(0x1p32) - 1) * 100;
                *reinterpret_cast<pair*>(b + 4) = digits.dd[f4 >> 32];
                b += 6;
            }
        }
        else if (n < u32(1e8))
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
            b += 8;
        }
        else if (n < u64(0x1p32))
        {
            auto f0 = u64(10 * u64(0x1p57) / u64(1e9) + 1) * u32(n);
            *reinterpret_cast<pair*>(b) = digits.fd[f0 >> 57];
            b -= n < u32(1e9);
            auto f2 = (f0 & u64(0x1p57) - 1) * 100;
            *reinterpret_cast<pair*>(b + 2) = digits.dd[f2 >> 57];
            auto f4 = (f2 & u64(0x1p57) - 1) * 100;
            *reinterpret_cast<pair*>(b + 4) = digits.dd[f4 >> 57];
            auto f6 = (f4 & u64(0x1p57) - 1) * 100;
            *reinterpret_cast<pair*>(b + 6) = digits.dd[f6 >> 57];
            auto f8 = (f6 & u64(0x1p57) - 1) * 100;
            *reinterpret_cast<pair*>(b + 8) = digits.dd[f8 >> 57];
            b += 10;
        }
        else
        {
            u32 y = n % u32(1e8);
            n /= u32(1e8);

            // n is 2, 3, or 4 digits (if n < 10 it would have been handled above)
            if (n < u32(1e2))
            {
                *(pair*)b = digits.dd[n];
                b += 2;
            }
            else
            {
                auto f0 = u32(10 * 0x1p24 / 1e3 + 1) * n;
                *reinterpret_cast<pair*>(b) = digits.fd[f0 >> 24];
                b -= n < u32(1e3);
                auto f2 = (f0 & u32(0x1p24) - 1) * 100;
                *reinterpret_cast<pair*>(b + 2) = digits.dd[f2 >> 24];
                b += 4;
            }
            // do 8 digits
            auto f0 = u64(10 * u64(0x1p48) / u64(1e7) + 1) * y >> 16;
            *reinterpret_cast<pair*>(b) = digits.dd[f0 >> 32];
            auto f2 = (f0 & u64(0x1p32) - 1) * 100;
            *reinterpret_cast<pair*>(b + 2) = digits.dd[f2 >> 32];
            auto f4 = (f2 & u64(0x1p32) - 1) * 100;
            *reinterpret_cast<pair*>(b + 4) = digits.dd[f4 >> 32];
            auto f6 = (f4 & u64(0x1p32) - 1) * 100;
            *reinterpret_cast<pair*>(b + 6) = digits.dd[f6 >> 32];
            b += 8;
        }
        // do 8 digits
        auto f0 = u64(10 * u64(0x1p48) / u64(1e7) + 1) * z >> 16;
        *reinterpret_cast<pair*>(b) = digits.dd[f0 >> 32];
        auto f2 = (f0 & u64(0x1p32) - 1) * 100;
        *reinterpret_cast<pair*>(b + 2) = digits.dd[f2 >> 32];
        auto f4 = (f2 & u64(0x1p32) - 1) * 100;
        *reinterpret_cast<pair*>(b + 4) = digits.dd[f4 >> 32];
        auto f6 = (f4 & u64(0x1p32) - 1) * 100;
        *reinterpret_cast<pair*>(b + 6) = digits.dd[f6 >> 32];
        return b + 8;
    }
} 