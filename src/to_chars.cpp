//#include <charconv>

/*
Copyright (c) 2017 James Edward Anhalt III - https://github.com/jeaiii/to_chars_jeaiii

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include <stdint.h>

struct pair { char t, o; };
#define P(T) T, '0',  T, '1', T, '2', T, '3', T, '4', T, '5', T, '6', T, '7', T, '8', T, '9'
static const pair s_pairs[] = { P('0'), P('1'), P('2'), P('3'), P('4'), P('5'), P('6'), P('7'), P('8'), P('9') };

#define W(N, I) *(pair*)&b[N] = s_pairs[I]
#define A(N) t = (uint64_t(1) << (32 + N / 5 * N * 53 / 16)) / uint32_t(1e##N) + 1 + N/6 - N/8, t *= u, t >>= N / 5 * N * 53 / 16, t += N / 6 * 4, W(0, t >> 32)
#define S(N) b[N] = char(uint64_t(10) * uint32_t(t) >> 32) + '0'
#define D(N) t = uint64_t(100) * uint32_t(t), W(N, t >> 32)

#define C0 b[0] = char(u) + '0'
#define C1 W(0, u)
#define C2 A(1), S(2)
#define C3 A(2), D(2)
#define C4 A(3), D(2), S(4)
#define C5 A(4), D(2), D(4)
#define C6 A(5), D(2), D(4), S(6)
#define C7 A(6), D(2), D(4), D(6)
#define C8 A(7), D(2), D(4), D(6), S(8)
#define C9 A(8), D(2), D(4), D(6), D(8)

#define L09(F) u < 100        ? L01(F) : L29(F)
#define L29(F) u < 1000000    ? L25(F) : L69(F)
#define L25(F) u < 10000      ? L23(F) : L45(F)
#define L69(F) u < 100000000  ? L67(F) : L89(F)
#define L03(F) u < 100        ? L01(F) : L23(F)

#define L01(F) u < 10         ? F(0) : F(1)
#define L23(F) u < 1000       ? F(2) : F(3)
#define L45(F) u < 100000     ? F(4) : F(5)
#define L67(F) u < 10000000   ? F(6) : F(7)
#define L89(F) u < 1000000000 ? F(8) : F(9)

template<bool COND> struct enabled;
template<> struct enabled<true> { typedef decltype(nullptr) type; };
#define ENABLED(COND) typename enabled<(COND)>::type = nullptr

#define LAST(N) (C##N, TERMINATE ? b[N + 1] = 0 : 0, b + N + 1)

template<bool TERMINATE, class T, ENABLED(sizeof(T) <= 4)>
__forceinline char* to_chars_jeaiii(char* b, T i)
{
    uint64_t t;
    uint32_t u = i < T(0) ? *b++ = '-', 0 - uint32_t(int32_t(i)) : uint32_t(i);
    return L09(LAST);
}

#define PART(N) (C##N, b += N + 1)

template<bool TERMINATE, class T, ENABLED(sizeof(T) == 8)>
__forceinline char* to_chars_jeaiii(char* b, T i)
{
    uint64_t t;
    uint64_t n = i < T(0) ? *b++ = '-', 0 - uint64_t(i) : uint64_t(i);
    uint32_t u = uint32_t(n);

    if (u == n)
        return L09(LAST);

    uint64_t a = n / 100000000;
    u = uint32_t(a);

    if (u == a)
    {
        L09(PART);
    }
    else
    {
        u = uint32_t(a / 100000000);
        L03(PART);
        u = a % 100000000;
        PART(7);
    }

    u = n % 100000000;
    return LAST(7);
}

void u32toa_to_chars_jeaiii(uint32_t n, char* b) { to_chars_jeaiii<true>(b, n); }
void i32toa_to_chars_jeaiii(int32_t n, char* b) { to_chars_jeaiii<true>(b, n); }
void u64toa_to_chars_jeaiii(uint64_t n, char* b) { to_chars_jeaiii<true>(b, n); }
void i64toa_to_chars_jeaiii(int64_t n, char* b) { to_chars_jeaiii<true>(b, n); }

#include <cstring>
enum class errc { value_too_large = 1 };
struct to_chars_result { char* ptr; errc ec; };

template<class T>
inline to_chars_result to_chars_jeaiii(char *first, char* last, T value)
{
    if (last - first < 20)
    {
        char temp[20];
        size_t n = to_chars_jeaiii<false>(temp, value) - temp;
        if (last - first < n)
            return { last,  errc::value_too_large };
        memcpy(first, temp, n);
        return { first + n, {} };
    }
    return { to_chars_jeaiii<false>(first, value), {} };
}

to_chars_result to_chars(char *first, char* last, signed char value) { return to_chars_jeaiii(first, last, value); }
