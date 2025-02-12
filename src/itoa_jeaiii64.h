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

namespace jeaiii64
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

    template<class T> __forceinline
    char* to_text_from_integer(char* b, T n)
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

            auto f0 = u64(10 * u64(0x1p58) / u64(1e9) + 1) * u32(n) >> 26;
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
            auto f0 = u64(10 * u64(0x1p58) / u64(1e9) + 1) * u32(n) >> 26;
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