#include "test.h"
#include <type_traits>
#include <concepts>
#include <utility>
#include <cstring>

#if defined(__clang__) || (defined(__GNUC__) && defined(__llvm__))
#define RTC_ITOA_CLANG 1
#elif defined(_MSC_VER)
#pragma warning(disable : 4820)
#pragma warning(disable : 4371)
#define RTC_ITOA_MSVC 1
#elif defined(__GNUC__) && !defined(__clang__)
#define RTC_ITOA_GNUCXX 1
#endif

#if defined(RTC_ITOA_MSVC)
#define RTC_ITOA_VISUAL_STUDIO 1
#if defined(RTC_ITOA_CLANG)
#define RTC_ITOA_CLANG_VISUAL_STUDIO 1
#else
#define RTC_ITOA_REGULAR_VISUAL_STUDIO 1
#endif
#endif

#if (defined(__x86_64__) || defined(_M_AMD64)) && !defined(_M_ARM64EC)
#define RTC_ITOA_IS_X86_64 1
#else
#define RTC_ITOA_IS_ARM64 1
#endif

#define RTC_ITOA_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)

#if defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__)) || defined(TARGET_OS_MAC)
#define RTC_ITOA_MAC 1
#define RTC_MAC
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
#define RTC_ITOA_LINUX 1
#define RTC_LZCNT
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#define RTC_ITOA_WIN 1
#define RTC_LZCNT
#else
#error "Undetected platform."
#endif

#if defined(__has_builtin)
#define RTC_ITOA_HAS_BUILTIN(x) __has_builtin(x)
#else
#define RTC_ITOA_HAS_BUILTIN(x) 0
#endif

#if !defined(RTC_ITOA_LIKELY)
#define RTC_ITOA_LIKELY(...) (__VA_ARGS__) [[likely]]
#endif

#if !defined(RTC_ITOA_UNLIKELY)
#define RTC_ITOA_UNLIKELY(...) (__VA_ARGS__) [[unlikely]]
#endif

#if !defined(RTC_ITOA_ELSE_UNLIKELY)
#define RTC_ITOA_ELSE_UNLIKELY(...) __VA_ARGS__ [[unlikely]]
#endif

#if defined(RTC_ITOA_GNUCXX) || defined(RTC_ITOA_CLANG)
#define RTC_ITOA_ASSUME(x) \
		do { \
			if (!(x)) \
				__builtin_unreachable(); \
		} while (0)
#elif defined(RTC_ITOA_MSVC)
#include <intrin.h>
#define RTC_ITOA_ASSUME(x) __assume(x)
#else
#define RTC_ITOA_ASSUME(x) (( void )0)
#endif

#if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L
#define RTC_ITOA_HAS_INLINE_VARIABLE 1
#elif __cplusplus >= 201703L
#define RTC_ITOA_HAS_INLINE_VARIABLE 1
#elif defined(RTC_ITOA_MSVC) && RTC_ITOA_MSVC >= 1912 && _MSVC_LANG >= 201703L
#define RTC_ITOA_HAS_INLINE_VARIABLE 1
#else
#define RTC_ITOA_HAS_INLINE_VARIABLE 0
#endif

#if RTC_ITOA_HAS_INLINE_VARIABLE
#define RTC_ITOA_INLINE_VARIABLE inline constexpr
#else
#define RTC_ITOA_INLINE_VARIABLE static constexpr
#endif

#if defined(RTC_ITOA_MSVC)
#define RTC_ITOA_INLINE [[msvc::forceinline]] inline
#define RTC_ITOA_NON_GCC_INLINE [[msvc::forceinline]] inline
#define RTC_ITOA_CLANG_MACOS_INLINE
#define RTC_ITOA_CLANG_INLINE
#elif defined(RTC_ITOA_CLANG)
#if defined(RTC_ITOA_MAC)
#define RTC_ITOA_CLANG_MACOS_INLINE inline __attribute__((always_inline))
#else
#define RTC_ITOA_CLANG_MACOS_INLINE
#endif
#define RTC_ITOA_NON_GCC_INLINE inline __attribute__((always_inline))
#define RTC_ITOA_NO_INLINE __attribute__((noinline))
#define RTC_ITOA_INLINE inline __attribute__((always_inline))
#define RTC_ITOA_CLANG_INLINE inline __attribute__((always_inline))
#elif defined(RTC_ITOA_GNUCXX)
#define RTC_ITOA_INLINE inline __attribute__((always_inline))
#define RTC_ITOA_NON_GCC_INLINE
#define RTC_ITOA_CLANG_INLINE
#define RTC_ITOA_CLANG_MACOS_INLINE
#endif

namespace jsonifier {
	namespace concepts {

		template<typename value_type>
		concept integer_t = std::integral<std::remove_cvref_t<value_type>> && !std::floating_point<std::remove_cvref_t<value_type>>;

		template<typename value_type>
		concept signed_t = std::signed_integral<std::remove_cvref_t<value_type>> ;

		template<typename value_type>
		concept unsigned_t = std::unsigned_integral<std::remove_cvref_t<value_type>> ;

		template<typename value_type>
		concept uns64_t = sizeof(std::remove_cvref_t<value_type>) == 8 && unsigned_t<value_type>;

		template<typename value_type>
		concept sig64_t = sizeof(std::remove_cvref_t<value_type>) == 8 && signed_t<value_type>;

		template<typename value_type>
		concept uns32_t = sizeof(std::remove_cvref_t<value_type>) == 4 && unsigned_t<value_type>;

		template<typename value_type>
		concept sig32_t = sizeof(std::remove_cvref_t<value_type>) == 4 && signed_t<value_type>;
	}
}

using string_buffer_ptr = char*;

namespace simd_internal {
#if defined(RTC_ITOA_MSVC)

	RTC_ITOA_INLINE uint32_t lzcnt(const uint32_t value) noexcept {
		return _lzcnt_u32(value);
	}

	RTC_ITOA_INLINE uint64_t lzcnt(const uint64_t value) noexcept {
		return _lzcnt_u64(value);
	}

#elif defined(RTC_ITOA_MAC)

	template<jsonifier::concepts::uns32_t value_type> RTC_ITOA_INLINE value_type lzcnt(const value_type value) noexcept {
		return __builtin_clz(value);
	}

	template<jsonifier::concepts::uns64_t value_type> RTC_ITOA_INLINE value_type lzcnt(const value_type value) noexcept {
		return __builtin_clzll(value);
	}

#else

	template<std::unsigned_integral value_type> RTC_ITOA_INLINE constexpr value_type lzcnt(const value_type value) noexcept {
		if (value == 0) {
			return sizeof(value_type) * 8;
		}

		value_type count{};
		value_type mask{ static_cast<value_type>(1) << (std::numeric_limits<value_type>::digits - 1) };

		while ((value & mask) == 0) {
			++count;
			mask >>= 1;
		}

		return count;
	}

#endif

}

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

alignas(2) RTC_ITOA_INLINE_VARIABLE char charTable1[]{ 0x30u, 0x31u, 0x32u, 0x33u, 0x34u, 0x35u, 0x36u, 0x37u, 0x38u, 0x39u };

alignas(2) RTC_ITOA_INLINE_VARIABLE char charTable2[]{ 0x30u, 0x30u, 0x30u, 0x31u, 0x30u, 0x32u, 0x30u, 0x33u, 0x30u, 0x34u, 0x30u, 0x35u, 0x30u, 0x36u, 0x30u, 0x37u, 0x30u,
	0x38u, 0x30u, 0x39u, 0x31u, 0x30u, 0x31u, 0x31u, 0x31u, 0x32u, 0x31u, 0x33u, 0x31u, 0x34u, 0x31u, 0x35u, 0x31u, 0x36u, 0x31u, 0x37u, 0x31u, 0x38u, 0x31u, 0x39u, 0x32u,
	0x30u, 0x32u, 0x31u, 0x32u, 0x32u, 0x32u, 0x33u, 0x32u, 0x34u, 0x32u, 0x35u, 0x32u, 0x36u, 0x32u, 0x37u, 0x32u, 0x38u, 0x32u, 0x39u, 0x33u, 0x30u, 0x33u, 0x31u, 0x33u,
	0x32u, 0x33u, 0x33u, 0x33u, 0x34u, 0x33u, 0x35u, 0x33u, 0x36u, 0x33u, 0x37u, 0x33u, 0x38u, 0x33u, 0x39u, 0x34u, 0x30u, 0x34u, 0x31u, 0x34u, 0x32u, 0x34u, 0x33u, 0x34u,
	0x34u, 0x34u, 0x35u, 0x34u, 0x36u, 0x34u, 0x37u, 0x34u, 0x38u, 0x34u, 0x39u, 0x35u, 0x30u, 0x35u, 0x31u, 0x35u, 0x32u, 0x35u, 0x33u, 0x35u, 0x34u, 0x35u, 0x35u, 0x35u,
	0x36u, 0x35u, 0x37u, 0x35u, 0x38u, 0x35u, 0x39u, 0x36u, 0x30u, 0x36u, 0x31u, 0x36u, 0x32u, 0x36u, 0x33u, 0x36u, 0x34u, 0x36u, 0x35u, 0x36u, 0x36u, 0x36u, 0x37u, 0x36u,
	0x38u, 0x36u, 0x39u, 0x37u, 0x30u, 0x37u, 0x31u, 0x37u, 0x32u, 0x37u, 0x33u, 0x37u, 0x34u, 0x37u, 0x35u, 0x37u, 0x36u, 0x37u, 0x37u, 0x37u, 0x38u, 0x37u, 0x39u, 0x38u,
	0x30u, 0x38u, 0x31u, 0x38u, 0x32u, 0x38u, 0x33u, 0x38u, 0x34u, 0x38u, 0x35u, 0x38u, 0x36u, 0x38u, 0x37u, 0x38u, 0x38u, 0x38u, 0x39u, 0x39u, 0x30u, 0x39u, 0x31u, 0x39u,
	0x32u, 0x39u, 0x33u, 0x39u, 0x34u, 0x39u, 0x35u, 0x39u, 0x36u, 0x39u, 0x37u, 0x39u, 0x38u, 0x39u, 0x39u };

RTC_ITOA_INLINE_VARIABLE uint8_t digitCounts[]{ 19, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16, 16, 16, 15, 15, 15, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10,
	10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1 };

RTC_ITOA_INLINE_VARIABLE uint64_t digitCountThresholds[]{ 0ull, 9ull, 99ull, 999ull, 9999ull, 99999ull, 999999ull, 9999999ull, 99999999ull, 999999999ull, 9999999999ull,
	99999999999ull, 999999999999ull, 9999999999999ull, 99999999999999ull, 999999999999999ull, 9999999999999999ull, 99999999999999999ull, 999999999999999999ull,
	9999999999999999999ull };

RTC_ITOA_INLINE uint64_t fastDigitCount(const uint64_t inputValue) {
	const uint64_t originalDigitCount{ static_cast<uint64_t>(digitCounts[simd_internal::lzcnt(inputValue)]) };
	return originalDigitCount + static_cast<uint64_t>(inputValue > digitCountThresholds[originalDigitCount]);
}

RTC_ITOA_INLINE static string_buffer_ptr length1(string_buffer_ptr buf, const uint64_t value) noexcept {
	std::memcpy(buf, charTable1 + value, 1);
	return buf + 1;
}

RTC_ITOA_INLINE static string_buffer_ptr length2(string_buffer_ptr buf, const uint64_t value) noexcept {
	std::memcpy(buf, charTable2 + value * 2, 2);
	return buf + 2;
}

RTC_ITOA_INLINE static string_buffer_ptr length3(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t aa = (value * 5243) >> 19;
	std::memcpy(buf, charTable1 + aa, 2);
	std::memcpy(buf + 1, charTable2 + (value - aa * 100) * 2, 2);
	return buf + 3;
}

RTC_ITOA_INLINE static string_buffer_ptr length4(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t aa = (value * 5243) >> 19;
	std::memcpy(buf, charTable2 + aa * 2, 2);
	std::memcpy(buf + 2, charTable2 + (value - aa * 100) * 2, 2);
	return buf + 4;
}

RTC_ITOA_INLINE static string_buffer_ptr length5(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t aa = (value * 429497) >> 32;
	const uint64_t bbcc = value - aa * 10000;
	const uint64_t bb = (bbcc * 5243) >> 19;
	std::memcpy(buf, charTable1 + aa, 2);
	std::memcpy(buf + 1, charTable2 + bb * 2, 2);
	std::memcpy(buf + 3, charTable2 + (bbcc - bb * 100) * 2, 2);
	return buf + 5;
}

RTC_ITOA_INLINE static string_buffer_ptr length6(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t aa = (value * 429497) >> 32;
	const uint64_t bbcc = value - aa * 10000;
	const uint64_t bb = (bbcc * 5243) >> 19;
	std::memcpy(buf, charTable2 + aa * 2, 2);
	std::memcpy(buf + 2, charTable2 + bb * 2, 2);
	std::memcpy(buf + 4, charTable2 + (bbcc - bb * 100) * 2, 2);
	return buf + 6;
}

RTC_ITOA_INLINE static string_buffer_ptr length7(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t aabb = (value * 109951163) >> 40;
	const uint64_t ccdd = value - aabb * 10000;
	const uint64_t aa = (aabb * 5243) >> 19;
	const uint64_t cc = (ccdd * 5243) >> 19;
	std::memcpy(buf, charTable1 + aa, 2);
	std::memcpy(buf + 1, charTable2 + (aabb - aa * 100) * 2, 2);
	std::memcpy(buf + 3, charTable2 + cc * 2, 2);
	std::memcpy(buf + 5, charTable2 + (ccdd - cc * 100) * 2, 2);
	return buf + 7;
}

RTC_ITOA_INLINE static string_buffer_ptr length8(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t aabb = (value * 109951163) >> 40;
	const uint64_t ccdd = value - aabb * 10000;
	const uint64_t aa = (aabb * 5243) >> 19;
	const uint64_t cc = (ccdd * 5243) >> 19;
	std::memcpy(buf, charTable2 + aa * 2, 2);
	std::memcpy(buf + 2, charTable2 + (aabb - aa * 100) * 2, 2);
	std::memcpy(buf + 4, charTable2 + cc * 2, 2);
	std::memcpy(buf + 6, charTable2 + (ccdd - cc * 100) * 2, 2);
	return buf + 8;
}

RTC_ITOA_INLINE static string_buffer_ptr length9(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t high = value / 100000000ull;
	const uint64_t low = value - high * 100000000ull;
	const uint64_t aabb = (low * 109951163ull) >> 40ull;
	const uint64_t ccdd = low - aabb * 10000ull;
	const uint64_t aa = (aabb * 5243ull) >> 19ull;
	const uint64_t bb = aabb - aa * 100ull;
	const uint64_t cc = (ccdd * 5243ull) >> 19ull;
	std::memcpy(buf, charTable1 + high, 1ull);
	std::memcpy(buf + 1ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 3ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 5ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 7ull, charTable2 + (ccdd - cc * 100ull) * 2ull, 2ull);
	return buf + 9ull;
}

RTC_ITOA_INLINE static string_buffer_ptr length10(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t high = value / 100000000ull;
	const uint64_t low = value - high * 100000000ull;
	const uint64_t aabb = (low * 109951163ull) >> 40ull;
	const uint64_t ccdd = low - aabb * 10000ull;
	const uint64_t aa = (aabb * 5243ull) >> 19ull;
	const uint64_t bb = aabb - aa * 100ull;
	const uint64_t cc = (ccdd * 5243ull) >> 19ull;
	std::memcpy(buf, charTable2 + high * 2ull, 2ull);
	std::memcpy(buf + 2ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 4ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 6ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 8ull, charTable2 + (ccdd - cc * 100ull) * 2ull, 2ull);
	return buf + 10ull;
}

RTC_ITOA_INLINE static string_buffer_ptr length11(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t high = value / 100000000ull;
	const uint64_t low = value - high * 100000000ull;
	uint64_t aa = (high * 5243ull) >> 19ull;
	uint64_t bb = high - aa * 100ull;
	const uint64_t aabb = (low * 109951163ull) >> 40ull;
	const uint64_t ccdd = low - aabb * 10000ull;
	std::memcpy(buf, charTable1 + aa, 1ull);
	std::memcpy(buf + 1ull, charTable2 + bb * 2ull, 2ull);
	aa = (aabb * 5243ull) >> 19ull;
	bb = aabb - aa * 100ull;
	const uint64_t cc = (ccdd * 5243ull) >> 19ull;
	std::memcpy(buf + 3ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 5ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 7ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 9ull, charTable2 + (ccdd - cc * 100ull) * 2ull, 2ull);
	return buf + 11ull;
}

RTC_ITOA_INLINE static string_buffer_ptr length12(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t high = value / 100000000ull;
	const uint64_t low = value - high * 100000000ull;
	uint64_t aa = (high * 5243ull) >> 19ull;
	uint64_t bb = high - aa * 100ull;
	std::memcpy(buf, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 2ull, charTable2 + bb * 2ull, 2ull);
	const uint64_t aabb = (low * 109951163ull) >> 40ull;
	const uint64_t ccdd = low - aabb * 10000ull;
	aa = (aabb * 5243ull) >> 19ull;
	bb = aabb - aa * 100ull;
	const uint64_t cc = (ccdd * 5243ull) >> 19ull;
	std::memcpy(buf + 4ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 6ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 8ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 10ull, charTable2 + (ccdd - cc * 100ull) * 2ull, 2ull);
	return buf + 12ull;
}

RTC_ITOA_INLINE static string_buffer_ptr length13(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t high = value / 100000000ull;
	const uint64_t low = value - high * 100000000ull;
	uint64_t aa = (high * 429497ull) >> 32ull;
	const uint64_t bbcc = high - aa * 10000ull;
	uint64_t bb = (bbcc * 5243ull) >> 19ull;
	uint64_t cc = bbcc - bb * 100ull;
	std::memcpy(buf, charTable1 + aa, 1ull);
	std::memcpy(buf + 1ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 3ull, charTable2 + cc * 2ull, 2ull);
	const uint64_t aabb = (low * 109951163ull) >> 40ull;
	const uint64_t ccdd = low - aabb * 10000ull;
	aa = (aabb * 5243ull) >> 19ull;
	bb = aabb - aa * 100ull;
	cc = (ccdd * 5243ull) >> 19ull;
	std::memcpy(buf + 5ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 7ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 9ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 11ull, charTable2 + (ccdd - cc * 100ull) * 2ull, 2ull);
	return buf + 13ull;
}

RTC_ITOA_INLINE static string_buffer_ptr length14(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t high = value / 100000000ull;
	const uint64_t low = value - high * 100000000ull;
	const uint64_t aabb = (low * 109951163ull) >> 40ull;
	const uint64_t ccdd = low - aabb * 10000ull;
	uint64_t aa = (high * 429497ull) >> 32ull;
	const uint64_t bbcc = high - aa * 10000ull;
	uint64_t bb = (bbcc * 5243ull) >> 19ull;
	uint64_t cc = bbcc - bb * 100ull;
	std::memcpy(buf, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 2ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 4ull, charTable2 + cc * 2ull, 2ull);
	aa = (aabb * 5243ull) >> 19ull;
	bb = aabb - aa * 100ull;
	cc = (ccdd * 5243ull) >> 19ull;
	std::memcpy(buf + 6ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 8ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 10ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 12ull, charTable2 + (ccdd - cc * 100ull) * 2ull, 2ull);
	return buf + 14ull;
}

RTC_ITOA_INLINE static string_buffer_ptr length15(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t high = value / 100000000ull;
	const uint64_t low = value - high * 100000000ull;
	uint64_t aabb = (high * 109951163ull) >> 40ull;
	uint64_t ccdd = high - aabb * 10000ull;
	uint64_t aa = (aabb * 5243ull) >> 19ull;
	uint64_t bb = aabb - aa * 100ull;
	uint64_t cc = (ccdd * 5243ull) >> 19ull;
	uint64_t dd = ccdd - cc * 100ull;
	std::memcpy(buf, charTable1 + aa, 1ull);
	std::memcpy(buf + 1ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 3ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 5ull, charTable2 + dd * 2ull, 2ull);
	aabb = (low * 109951163ull) >> 40ull;
	ccdd = low - aabb * 10000ull;
	aa = (aabb * 5243ull) >> 19ull;
	bb = aabb - aa * 100ull;
	cc = (ccdd * 5243ull) >> 19ull;
	dd = ccdd - cc * 100ull;
	std::memcpy(buf + 7ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 9ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 11ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 13ull, charTable2 + dd * 2ull, 2ull);
	return buf + 15ull;
}

RTC_ITOA_INLINE static string_buffer_ptr length16(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t high = value / 100000000ull;
	const uint64_t low = value - high * 100000000ull;
	uint64_t aabb = (high * 109951163ull) >> 40ull;
	uint64_t ccdd = high - aabb * 10000ull;
	uint64_t aa = (aabb * 5243ull) >> 19ull;
	uint64_t bb = aabb - aa * 100ull;
	uint64_t cc = (ccdd * 5243ull) >> 19ull;
	uint64_t dd = ccdd - cc * 100ull;
	std::memcpy(buf, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 2ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 4ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 6ull, charTable2 + dd * 2ull, 2ull);
	aabb = (low * 109951163ull) >> 40ull;
	ccdd = low - aabb * 10000ull;
	aa = (aabb * 5243ull) >> 19ull;
	bb = aabb - aa * 100ull;
	cc = (ccdd * 5243ull) >> 19ull;
	dd = ccdd - cc * 100ull;
	std::memcpy(buf + 8ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 10ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 12ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 14ull, charTable2 + dd * 2ull, 2ull);
	return buf + 16ull;
}

RTC_ITOA_INLINE static string_buffer_ptr length17(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t tmp = value / 100000000ull;
	const uint64_t low = value - tmp * 100000000ull;
	const uint64_t high = tmp / 10000ull;
	const uint64_t mid = tmp - high * 10000ull;
	const uint64_t aabb = (low * 109951163ull) >> 40ull;
	const uint64_t ccdd = low - aabb * 10000ull;
	uint64_t aa = (high * 429497ull) >> 32ull;
	const uint64_t bbcc = high - aa * 10000ull;
	uint64_t bb = (bbcc * 5243ull) >> 19ull;
	uint64_t cc = bbcc - bb * 100ull;
	std::memcpy(buf, charTable1 + aa, 1ull);
	std::memcpy(buf + 1ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 3ull, charTable2 + cc * 2ull, 2ull);
	aa = (mid * 5243ull) >> 19ull;
	bb = mid - aa * 100ull;
	std::memcpy(buf + 5ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 7ull, charTable2 + bb * 2ull, 2ull);
	aa = (aabb * 5243ull) >> 19ull;
	bb = aabb - aa * 100ull;
	cc = (ccdd * 5243ull) >> 19ull;
	const uint64_t dd = ccdd - cc * 100ull;
	std::memcpy(buf + 9ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 11ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 13ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 15ull, charTable2 + dd * 2ull, 2ull);
	return buf + 17ull;
}

RTC_ITOA_INLINE static string_buffer_ptr length18(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t tmp = value / 100000000ull;
	const uint64_t low = value - tmp * 100000000ull;
	const uint64_t high = tmp / 10000ull;
	const uint64_t mid = tmp - high * 10000ull;
	const uint64_t aabb = (low * 109951163ull) >> 40ull;
	const uint64_t ccdd = low - aabb * 10000ull;
	uint64_t aa = (high * 429497ull) >> 32ull;
	const uint64_t bbcc = high - aa * 10000ull;
	uint64_t bb = (bbcc * 5243ull) >> 19ull;
	uint64_t cc = bbcc - bb * 100ull;
	std::memcpy(buf, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 2ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 4ull, charTable2 + cc * 2ull, 2ull);
	aa = (mid * 5243ull) >> 19ull;
	bb = mid - aa * 100ull;
	std::memcpy(buf + 6ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 8ull, charTable2 + bb * 2ull, 2ull);
	aa = (aabb * 5243ull) >> 19ull;
	bb = aabb - aa * 100ull;
	cc = (ccdd * 5243ull) >> 19ull;
	const uint64_t dd = ccdd - cc * 100ull;
	std::memcpy(buf + 10ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 12ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 14ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 16ull, charTable2 + dd * 2ull, 2ull);
	return buf + 18ull;
}

RTC_ITOA_INLINE static string_buffer_ptr length19(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t tmp = value / 100000000ull;
	const uint64_t low = value - tmp * 100000000ull;
	const uint64_t high = tmp / 10000ull;
	const uint64_t mid = tmp - high * 10000ull;
	uint64_t aabb = (high * 109951163ull) >> 40ull;
	uint64_t ccdd = high - aabb * 10000ull;
	uint64_t aa = (aabb * 5243ull) >> 19ull;
	uint64_t bb = aabb - aa * 100ull;
	uint64_t cc = (ccdd * 5243ull) >> 19ull;
	uint64_t dd = ccdd - cc * 100ull;
	std::memcpy(buf, charTable1 + aa, 1ull);
	std::memcpy(buf + 1ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 3ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 5ull, charTable2 + dd * 2ull, 2ull);
	aa = (mid * 5243ull) >> 19ull;
	bb = mid - aa * 100ull;
	std::memcpy(buf + 7ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 9ull, charTable2 + bb * 2ull, 2ull);
	aabb = (low * 109951163ull) >> 40ull;
	ccdd = low - aabb * 10000ull;
	aa = (aabb * 5243ull) >> 19ull;
	bb = aabb - aa * 100ull;
	cc = (ccdd * 5243ull) >> 19ull;
	dd = ccdd - cc * 100ull;
	std::memcpy(buf + 11ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 13ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 15ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 17ull, charTable2 + dd * 2ull, 2ull);
	return buf + 19ull;
}

RTC_ITOA_INLINE static string_buffer_ptr length20(string_buffer_ptr buf, const uint64_t value) noexcept {
	const uint64_t tmp = value / 100000000ull;
	const uint64_t low = value - tmp * 100000000ull;
	const uint64_t high = tmp / 10000ull;
	const uint64_t mid = tmp - high * 10000ull;
	uint64_t aabb = (high * 109951163ull) >> 40ull;
	uint64_t ccdd = high - aabb * 10000ull;
	uint64_t aa = (aabb * 5243ull) >> 19ull;
	uint64_t bb = aabb - aa * 100ull;
	uint64_t cc = (ccdd * 5243ull) >> 19ull;
	uint64_t dd = ccdd - cc * 100ull;
	std::memcpy(buf, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 2ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 4ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 6ull, charTable2 + dd * 2ull, 2ull);
	aa = (mid * 5243ull) >> 19ull;
	bb = mid - aa * 100ull;
	std::memcpy(buf + 8ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 10ull, charTable2 + bb * 2ull, 2ull);
	aabb = (low * 109951163ull) >> 40ull;
	ccdd = low - aabb * 10000ull;
	aa = (aabb * 5243ull) >> 19ull;
	bb = aabb - aa * 100ull;
	cc = (ccdd * 5243ull) >> 19ull;
	dd = ccdd - cc * 100ull;
	std::memcpy(buf + 12ull, charTable2 + aa * 2ull, 2ull);
	std::memcpy(buf + 14ull, charTable2 + bb * 2ull, 2ull);
	std::memcpy(buf + 16ull, charTable2 + cc * 2ull, 2ull);
	std::memcpy(buf + 18ull, charTable2 + dd * 2ull, 2ull);
	return buf + 20ull;
}

template<size_t index> RTC_ITOA_INLINE static string_buffer_ptr impl64(string_buffer_ptr buf, const uint64_t value) noexcept {
	if constexpr (index == 1) {
		return length1(buf, value);
	}
	else if constexpr (index == 2) {
		return length2(buf, value);
	}
	else if constexpr (index == 3) {
		return length3(buf, value);
	}
	else if constexpr (index == 4) {
		return length4(buf, value);
	}
	else if constexpr (index == 5) {
		return length5(buf, value);
	}
	else if constexpr (index == 6) {
		return length6(buf, value);
	}
	else if constexpr (index == 7) {
		return length7(buf, value);
	}
	else if constexpr (index == 8) {
		return length8(buf, value);
	}
	else if constexpr (index == 9) {
		return length9(buf, value);
	}
	else if constexpr (index == 10) {
		return length10(buf, value);
	}
	else if constexpr (index == 11) {
		return length11(buf, value);
	}
	else if constexpr (index == 12) {
		return length12(buf, value);
	}
	else if constexpr (index == 13) {
		return length13(buf, value);
	}
	else if constexpr (index == 14) {
		return length14(buf, value);
	}
	else if constexpr (index == 15) {
		return length15(buf, value);
	}
	else if constexpr (index == 16) {
		return length16(buf, value);
	}
	else if constexpr (index == 17) {
		return length17(buf, value);
	}
	else if constexpr (index == 18) {
		return length18(buf, value);
	}
	else if constexpr (index == 19) {
		return length19(buf, value);
	}
	else if constexpr (index == 20) {
		return length20(buf, value);
	}
	else {
		std::unreachable();
	}
}

template<jsonifier::concepts::uns64_t value_type_new, size_t... indices>
RTC_ITOA_INLINE string_buffer_ptr impl64(string_buffer_ptr buf, const value_type_new value, size_t index, std::index_sequence<indices...>) noexcept {
	char* ptr{};
	((index == indices && (ptr = impl64<indices>(buf, value))), ...);
	return ptr;
}

template<jsonifier::concepts::uns64_t value_type_new> RTC_ITOA_INLINE static string_buffer_ptr toChars(string_buffer_ptr buf, const value_type_new value) noexcept {
	const uint64_t index{ fastDigitCount(value) };
	return impl64(buf, value, index, std::make_index_sequence<21>{});
}

template<jsonifier::concepts::sig64_t value_type_new> RTC_ITOA_INLINE static string_buffer_ptr toChars(string_buffer_ptr buf, const value_type_new value) noexcept {
	*buf = '-';
	return toChars(buf + (value < 0), static_cast<const uint64_t>(value ^ (value >> 63)) - (value >> 63));
}
#include <iostream>

RTC_ITOA_INLINE void u32toa_rtc_itoa(uint32_t value, char* buffer) {
	auto newPtr = toChars(buffer, static_cast<uint64_t>(value));
	buffer[newPtr - buffer] = '\0';
}

RTC_ITOA_INLINE void i32toa_rtc_itoa(int32_t value, char* buffer) {
	auto newPtr = toChars(buffer, static_cast<int64_t>(value));
	buffer[newPtr - buffer] = '\0';
}

RTC_ITOA_INLINE void u64toa_rtc_itoa(uint64_t value, char* buffer) {
	auto newPtr = toChars(buffer, value);
	buffer[newPtr - buffer] = '\0';
}

RTC_ITOA_INLINE void i64toa_rtc_itoa(int64_t value, char* buffer) {
	auto newPtr = toChars(buffer, value);
	buffer[newPtr - buffer] = '\0';
}

REGISTER_TEST(rtc_itoa);