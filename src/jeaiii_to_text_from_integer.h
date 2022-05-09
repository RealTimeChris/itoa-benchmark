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

        //https://godbolt.org/z/b5qe1bTev

    __forceinline char* to_text_n32(char* b, u32 n) noexcept
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

    __forceinline char* to_text_n64(char* b, u64 n) noexcept
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
