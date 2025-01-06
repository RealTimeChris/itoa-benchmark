/*
MIT License

Copyright(c) 2017 James Edward Anhalt III (jeaiii)
https://github.com/jeaiii/itoa

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "test.h"

#define NDEBUG
#include "fmt/compile.h"

void u32toa_fmt(uint32_t n, char* b) { *fmt::format_to(b, FMT_COMPILE("{}"), n) = 0; }
void i32toa_fmt(int32_t n, char* b) { *fmt::format_to(b, FMT_COMPILE("{}"), n) = 0; }
void u64toa_fmt(uint64_t n, char* b) { *fmt::format_to(b, FMT_COMPILE("{}"), n) = 0; }
void i64toa_fmt(int64_t n, char* b) { *fmt::format_to(b, FMT_COMPILE("{}"), n) = 0; }
////REGISTER_TEST(fmt); 
