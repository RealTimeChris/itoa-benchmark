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

#include <stdint.h>


struct pair { char t, o; };
#define P(T) T, '0',  T, '1', T, '2', T, '3', T, '4', T, '5', T, '6', T, '7', T, '8', T, '9'
static const pair s_pairs[]{ P('0'), P('1'), P('2'), P('3'), P('4'), P('5'), P('6'), P('7'), P('8'), P('9') };
static const pair s_heads[]
{ 
 '0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0, '9', 0,
 P('1'), P('2'), P('3'), P('4'), P('5'), P('6'), P('7'), P('8'), P('9')
};

#define K(N) ((((uint64_t(1) << 57) + uint64_t(1e##N) - 1) / uint64_t(1e##N)))

void u64toa_jeaiii(uint64_t n, char* b);

void u64toa_jeaiii64(uint64_t n, char *b) {
    uint64_t t;

    n < 10000
        ? n < 100
        ? 
            (
            *(pair*)&b[0] = s_heads[n], b-= n < 10,
            b[2] = 0)
        :
            (t = K(2)*n, n = t >> 57,
            *(pair*)&b[0] = s_heads[n], b -= n < 10,
            t -= n << 57 - 0, t *= 25, *(pair*)&b[2] = s_pairs[t >> 57 - 2],
            b[4] = 0)
        : 
        n < 100000000
            ? n < 1000000
            ?
            (t = K(4)*n, n = t >> 57,
            *(pair*)&b[0] = s_heads[n], b -= n < 10,
            t -= n << 57 - 0, t *= 25, *(pair*)&b[2] = s_pairs[n = t >> 57 - 2],
            t -= n << 57 - 2, t *= 25, *(pair*)&b[4] = s_pairs[n = t >> 57 - 4],
            b[6] = 0)                
        :
            (t = K(6)*n, n = t >> 57,
            *(pair*)&b[0] = s_heads[n], b -= n < 10,
            t -= n << 57 - 0, t *= 25, *(pair*)&b[2] = s_pairs[n = t >> 57 - 2],
            t -= n << 57 - 2, t *= 25, *(pair*)&b[4] = s_pairs[n = t >> 57 - 4],
            t -= n << 57 - 4, t *= 25, *(pair*)&b[6] = s_pairs[n = t >> 57 - 6],
            b[8] = 0)
        :
        n < 1000000000000ull
            ? n < 10000000000ull
        ?
            (t = K(8)*n, n = t >> 57,
            *(pair*)&b[0] = s_heads[n], b -= n < 10,
            t -= n << 57 - 0, t *= 25, *(pair*)&b[2] = s_pairs[n = t >> 57 - 2],
            t -= n << 57 - 2, t *= 25, *(pair*)&b[4] = s_pairs[n = t >> 57 - 4],
            t -= n << 57 - 4, t *= 25, *(pair*)&b[6] = s_pairs[n = t >> 57 - 6],
            t -= n << 57 - 6, t *= 25, *(pair*)&b[8] = s_pairs[n = t >> 57 - 8],
            b[10] = 0)
        :
            (t = K(10)*n, n = t >> 57,
            *(pair*)&b[0] = s_heads[n], b -= n < 10,
            t -= n << 57 - 0, t *= 25, *(pair*)&b[2] = s_pairs[n = t >> 57 - 2],
            t -= n << 57 - 2, t *= 25, *(pair*)&b[4] = s_pairs[n = t >> 57 - 4],
            t -= n << 57 - 4, t *= 25, *(pair*)&b[6] = s_pairs[n = t >> 57 - 6],
            t -= n << 57 - 6, t *= 25, *(pair*)&b[8] = s_pairs[n = t >> 57 - 8],
            t -= n << 57 - 8, t *= 25, *(pair*)&b[10] = s_pairs[n = t >> 57 - 10],
            b[12] = 0)

        : (char)(u64toa_jeaiii(n, b), 0);
}

void i64toa_jeaiii64(int64_t n, char* b) {
    uint64_t u = n;
    if (n < 0) *b++ = '-', u = 0 - u;
    u64toa_jeaiii64(u, b);
}

static constexpr union
{
    char _[402]{
        "00010203040506070809"
        "10111213141516171819202122232425262728293031323334353637383940414243444546474849"
        "5051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899"
        "0\0001\0002\0003\0004\0005\0006\0007\0008\0009\000"
        "10111213141516171819202122232425262728293031323334353637383940414243444546474849"
        "5051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899"
        "\000"
    };
    struct
    {
        pair dd[100];
        pair fd[100];
        pair z;
    };
}
digits{ };

inline void u32toa_jeaiii64(uint32_t n, char* b) noexcept
{
    if (n < 100)
    {
        b[2] = 0, * (pair*)b = digits.fd[n];
    }
    else if (n < 1000000)
    {
        if (n < 10000)
        {
            auto t = (uint64_t(0x1p32) / uint64_t(1e2) + 1) * n;
            *(pair*)b = digits.fd[t >> 32], b -= n < 1000, b[4] = 0;
            t = uint32_t(t), t *= 100, * (pair*)&b[2] = digits.dd[t >> 32];
        }
        else
        {
            auto t = (uint64_t(0x1p32) / uint64_t(1e4) + 1) * n;
            *(pair*)b = digits.fd[t >> 32], b -= n < 100000, b[6] = 0;
            t = uint32_t(t), t *= 100, * (pair*)&b[2] = digits.dd[t >> 32];
            t = uint32_t(t), t *= 100, * (pair*)&b[4] = digits.dd[t >> 32];
        }
    }
    else {
        if (n < 100000000)
        {
            auto t = (uint64_t(0x1p48) / uint64_t(1e6) + 1) * n >> 16;
            *(pair*)b = digits.fd[t >> 32], b -= n < 10000000, b[8] = 0;
            t = uint32_t(t), t *= 100, * (pair*)&b[2] = digits.dd[t >> 32];
            t = uint32_t(t), t *= 100, * (pair*)&b[4] = digits.dd[t >> 32];
            t = uint32_t(t), t *= 100, * (pair*)&b[6] = digits.dd[t >> 32];
        }
        else
        {
            auto t = (uint64_t(0x1p58) / uint64_t(1e8) + 1) * n >> 26;
            *(pair*)b = digits.fd[t >> 32], b -= n < 1000000000, b[10] = 0;
            t = uint32_t(t), t *= 100, * (pair*)&b[2] = digits.dd[t >> 32];
            t = uint32_t(t), t *= 100, * (pair*)&b[4] = digits.dd[t >> 32];
            t = uint32_t(t), t *= 100, * (pair*)&b[6] = digits.dd[t >> 32];
            t = uint32_t(t), t *= 100, * (pair*)&b[8] = digits.dd[t >> 32];
        }
    }
}

void i32toa_jeaiii64(int32_t n, char* b) {
    uint32_t u = n < 0 ? *b++ = '-', 0 - uint32_t(n) : n;
    u32toa_jeaiii64(u, b);
}


//t <<= 7 + 0, t >>= 7 + 0, t *= 25, *(pair*)&b[2] = s_pairs[t >> 57 - 2],
