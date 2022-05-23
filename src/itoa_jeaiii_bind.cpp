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
#define CAT(_, ...) _ ## __VA_ARGS__
#define REGISTER_TEST_OVERLOAD(f) static Test CAT(gRegister, __LINE__)(STRINGIFY(f), f, f, f, f)


#include "int_to_chars_jeaiii.h"
#include "jeaiii_to_text32.h"
#include "jeaiii_to_text.h"

#if 0
namespace jeaiii32
{
    void to_text(uint32_t n, char* b) { *jeaiii::to_text_n32(b, n) = 0; }
    void to_text(int32_t n, char* b) { int_to_chars_jeaiii<void>(n, b); }
    void to_text(uint64_t n, char* b) { *jeaiii::to_text_n64(b, n) = 0; }
    void to_text(int64_t n, char* b) { int_to_chars_jeaiii<void>(n, b); }
}
REGISTER_TEST_OVERLOAD(jeaiii32::to_text);
#endif


#if 1
//static Test gRegister48("int_to_chars_jeaiii<void>", int_to_chars_jeaiii<void>, int_to_chars_jeaiii<void>, int_to_chars_jeaiii<void>, int_to_chars_jeaiii<void>);
#endif

#if 1
namespace jeaiii
{
    void to_text(uint32_t n, char* b) { *to_text_from_integer(b, n) = 0; }
    //void to_text(int32_t n, char* b) { int_to_chars_jeaiii<void>(n, b); }
    void to_text(uint64_t n, char* b) { *to_text_from_integer(b, n) = 0; }
    //void to_text(int64_t n, char* b) { int_to_chars_jeaiii<void>(n, b); }
}
//REGISTER_TEST_OVERLOAD(jeaiii::to_text);
#endif


void u32toa_jeaiii(uint32_t i, char* b);
void i32toa_jeaiii(int32_t i, char* b);
void u64toa_jeaiii(uint64_t i, char* b);
void i64toa_jeaiii(int64_t i, char* b);

REGISTER_TEST(jeaiii);

