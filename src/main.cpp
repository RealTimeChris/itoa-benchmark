#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <exception>
#include <limits>
#include <random>
#include <stdint.h>
#include <stdlib.h>
#include "resultfilename.h"
#include "rtc-64-bit.hpp"
#include "timer.h"
#include "test.h"

/*
Changes Copyright (c) 2018 James Edward Anhalt III - https://github.com/jeaiii
*/

const unsigned c_scale = 4096 * 4;

const int kTrial = 256 * 17;

template <typename T>
struct Traits {
};

template <>
struct Traits<uint32_t> {
    enum { kBufferSize = 11 };
    enum { kMaxDigit = 10 };
    static uint32_t Negate(uint32_t x) { return x; };
    static constexpr auto name = "u32toa";
    static constexpr auto categories = "[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 'URND LEN']";
};

template <>
struct Traits<int32_t> {
    enum { kBufferSize = 12 };
    enum { kMaxDigit = 10 };
    static int32_t Negate(int32_t x) { return -x; };
    static constexpr auto name = "i32toa";
    static constexpr auto categories = "[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 'URND LEN']";
};

template <>
struct Traits<uint64_t> {
    enum { kBufferSize = 21 };
    enum { kMaxDigit = 20 };
    static uint64_t Negate(uint64_t x) { return x; };
    static constexpr auto name = "u64toa";
    static constexpr auto categories = "[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 'URND LEN']";
};

template <>
struct Traits<int64_t> {
    enum { kBufferSize = 21 };
    enum { kMaxDigit = 19 };
    static int64_t Negate(int64_t x) { return -x; };
    static constexpr auto name = "i64toa";
    static constexpr auto categories = "[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 'URND LEN']";
};

template <typename T>
static void VerifyValue(T value, void(*f)(T, char*), void(*g)(T, char*), const char* fname, const char* gname) {
    char buffer1[Traits<T>::kBufferSize];
    char buffer2[Traits<T>::kBufferSize];

    f(value, buffer1);
    g(value, buffer2);

    if (strcmp(buffer1, buffer2) != 0) {
        printf("\nError: %s -> %s, %s -> %s\n", fname, buffer1, gname, buffer2);
        //throw std::exception();
    }
    //puts(buffer1);
}

template <typename T>
static void Verify(void(*f)(T, char*), void(*g)(T, char*), const char* fname, const char* gname) {
    printf("Verifying %s = %s ... ", fname, gname);

    // Boundary cases
    VerifyValue<T>(0, f, g, fname, gname);
    VerifyValue<T>(std::numeric_limits<T>::min(), f, g, fname, gname);
    VerifyValue<T>(std::numeric_limits<T>::max(), f, g, fname, gname);

    // 2^n - 1, 2^n, 10^n - 1, 10^n until overflow
    for (uint32_t power = 2; power <= 10; power += 8) {
        T i = 1, last;
        do {
            VerifyValue<T>(i - 1, f, g, fname, gname);
            VerifyValue<T>(i, f, g, fname, gname);
            if (std::numeric_limits<T>::min() < 0) {
                VerifyValue<T>(Traits<T>::Negate(i), f, g, fname, gname);
                VerifyValue<T>(Traits<T>::Negate(i + 1), f, g, fname, gname);
            }
            last = i;
            i *= power;
        } while (last < i);
    }

    printf("OK\n");
}

void VerifyAll() {
    const TestList& tests = TestManager::Instance().GetTests();

    // Find naive for verification
    const Test* naive = 0;
    for (TestList::const_iterator itr = tests.begin(); itr != tests.end(); ++itr)
    if (strcmp((*itr)->fname, "naive") == 0) {
        naive = *itr;
        break;
    }

    assert(naive != 0);

    for (TestList::const_iterator itr = tests.begin(); itr != tests.end(); ++itr) {
        if (strcmp((*itr)->fname, "null") != 0) {   // skip null
            try {
                Verify(naive->u32toa, (*itr)->u32toa, "naive_u32toa", (*itr)->fname);
                Verify(naive->i32toa, (*itr)->i32toa, "naive_i32toa", (*itr)->fname);
                Verify(naive->u64toa, (*itr)->u64toa, "naive_u64toa", (*itr)->fname);
                Verify(naive->i64toa, (*itr)->i64toa, "naive_i64toa", (*itr)->fname);
            }
            catch (...) {
            }
        }
    }
}

#if defined(_WIN32) || defined(_WIN64)
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

uint64_t TimeEnter() {
    int info[4];

#if defined(_WIN32) || defined(_WIN64)
    _ReadWriteBarrier();
    __cpuidex(info, 0, 0);
    uint64_t time = __rdtsc();
    _ReadWriteBarrier();
#else
    asm volatile ("cpuid" : "=a"(info[0]), "=b"(info[1]), "=c"(info[2]), "=d"(info[3]) : "a"(0), "c"(0));
    uint64_t time = __rdtsc();
#endif

    return time;
}

uint64_t TimeLeave() {
    int info[4];
    unsigned int aux;

#if defined(_WIN32) || defined(_WIN64)
    _ReadWriteBarrier();
    uint64_t time = __rdtscp(&aux);
    __cpuidex(info, 0, 0);
    _ReadWriteBarrier();
#else
    uint64_t time = __rdtscp(&aux);
    asm volatile ("cpuid" : "=a"(info[0]), "=b"(info[1]), "=c"(info[2]), "=d"(info[3]) : "a"(0), "c"(0));
#endif

    return time;
}



template<class T, size_t N>
uint64_t BenchData(void(*f)(T, char*), const T(&data)[N])
{
    char buffer[Traits<T>::kBufferSize];
    uint64_t duration = std::numeric_limits<uint64_t>::max();

    int trial = kTrial;
    do {

        uint64_t time = TimeEnter();
        for (size_t i = 0; i < N; i += 8) {
            f(data[i + 0], buffer);
            f(data[i + 1], buffer);
            f(data[i + 2], buffer);
            f(data[i + 3], buffer);
            f(data[i + 4], buffer);
            f(data[i + 5], buffer);
            f(data[i + 6], buffer);
            f(data[i + 7], buffer);
        }
        time = TimeLeave() - time;

        if (time < duration)
            duration = time, trial = kTrial;

    } while (--trial > 0);

    return duration;// / N;
}

void out_duration(FILE* fp, uint64_t t)
{
    t /= (c_scale / 8);
    if (t % 2 != 0) t += 1;
    fprintf(fp, "%f,", double(t) / 8);
}

template <typename T>
void BenchSequential(FILE* fp, void(*f)(T, char*), const char* type, const char* fname) {
    printf("Benchmarking sequential %-20s ... ", fname);

    static T data[c_scale];

    uint64_t durations[Traits<T>::kMaxDigit + 1];

    T start = 0;
    for (int digit = 0; ++digit <= Traits<T>::kMaxDigit;) {
        T end = (digit == Traits<T>::kMaxDigit) ? std::numeric_limits<T>::max() : (start + !start) * 10 - 1;
        T v = start;
        for (size_t i = 0; i < c_scale; ++i) {
            data[i] = v;
            v = v == end ? start : v + 1;
        }

        durations[digit] = BenchData(f, data);

        start = end + 1;
    }

    uint64_t min_d = durations[1];
    uint64_t max_d = durations[1];
    for (int digit = 1; digit <= Traits<T>::kMaxDigit; digit++) {
        out_duration(fp, durations[digit]);
        min_d = std::min(min_d, durations[digit]);
        max_d = std::max(max_d, durations[digit]);
    }

    printf("[%lld cc, %lld cc]\n", min_d, max_d);
}

template <class T, size_t N>
class RandomData {
public:
    static T(&GetData())[N] {
        static RandomData singleton;
        return singleton.m_data;
    }

    
private:
    RandomData()
    {
        T x = 0;
        T sign = 1;

        for (size_t i = 0; i < N; ++i) {
            if (x == 0)
                x = std::numeric_limits<T>::max();

            m_data[i] = x * sign;
            if (i % 64 == 0) 
                sign = Traits<T>::Negate(sign);
            x /= 10;
        }

        //std::random_device rd;
        std::mt19937 g(19700619);
        std::shuffle(m_data, m_data + N, g);
    }

    T m_data[N];
};

template <typename T>
void BenchRandom(FILE* fp, void(*f)(T, char*), const char* type, const char* fname) {
    printf("Benchmarking     random %-20s ... ", fname);

    uint64_t duration = BenchData(f, RandomData<T, c_scale>::GetData());

    out_duration(fp, duration);
    printf("%lld cc\n", duration);
}

template <typename T>
void Bench(FILE* fp, void(*f)(T, char*), const char* type, const char* fname)
{
    fprintf(fp, "{ name: '%s', data:[", fname);

    //Sleep(1);
    BenchSequential(fp, f, type, fname);
    //Sleep(1);
    BenchRandom(fp, f, type, fname);

    fprintf(fp, "]},\n");
}

template<class T> auto get_function(Test const* test);
template<> auto get_function<uint32_t>(Test const* test) { return test->u32toa; }
template<> auto get_function<int32_t>(Test const* test) { return test->i32toa; }
template<> auto get_function<uint64_t>(Test const* test) { return test->u64toa; }
template<> auto get_function<int64_t>(Test const* test) { return test->i64toa; }

template <typename T>
void Bench(FILE* fp, const TestList& tests)
{
    printf("\n%s\n", Traits<T>::name);

    fprintf(fp, "{ name: '%s', info: '" OPTIMIZE " " OS " " COMPILER "'", Traits<T>::name);
    fprintf(fp, ", categories: %s", Traits<T>::categories);
    fprintf(fp, ", data: [\n");

    for (TestList::const_iterator itr = tests.begin(); itr != tests.end(); ++itr)
    {
        //if (strcmp((*itr)->fname, "naive") != 0)
        {
            Bench(fp, get_function<T>(*itr), Traits<T>::name, (*itr)->fname);
        }
    }

    fprintf(fp, "]},\n");
}

void BenchAll() {
    // Try to write to /result path, where template.php exists
    FILE *fp;
    fp = fopen(RESULT_FILENAME, "w");

    const TestList& tests = TestManager::Instance().GetTests();

    fprintf(fp, "var tests=[");

    Bench<uint32_t>(fp, tests);
//    Bench<int32_t>(fp, tests);
    Bench<uint64_t>(fp, tests);
//    Bench<int64_t>(fp, tests);

    fprintf(fp, "];\n");
    fclose(fp);
}

int main() {
    // sort tests
    TestList& tests = TestManager::Instance().GetTests();

    VerifyAll();
    BenchAll();
}
