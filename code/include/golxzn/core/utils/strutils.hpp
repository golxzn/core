#pragma once

#include <string>
#include <vector>
#include <locale>
#include <codecvt>
#include <string_view>

#include "golxzn/core/aliases.hpp"
#include "golxzn/core/utils/traits.hpp"

namespace golxzn::core::utils {

template<class T> struct whitespaces {};

template<> struct whitespaces<char> {
	static constexpr std::basic_string_view<char> value{ " \t" };
};
template<> struct whitespaces<wchar_t> {
	static constexpr std::basic_string_view<wchar_t> value{ L" \t" };
};

template<class T>
static constexpr auto whitespaces_v{ whitespaces<T>::value };


[[nodiscard]] inline size_t size(const char *s) noexcept { return std::strlen(s); }
[[nodiscard]] inline size_t size(const wchar_t *s) noexcept { return std::wcslen(s); }
[[nodiscard]] inline size_t size(const std::string &s) noexcept { return s.size(); }
[[nodiscard]] inline size_t size(const std::wstring &s) noexcept { return s.size(); }
[[nodiscard]] constexpr size_t size(const std::string_view &s) noexcept { return s.size(); }
[[nodiscard]] constexpr size_t size(const std::wstring_view &s) noexcept { return s.size(); }
[[nodiscard]] constexpr size_t size(const char &) noexcept { return sizeof(char); }
[[nodiscard]] constexpr size_t size(const wchar_t &) noexcept { return sizeof(wchar_t); }
[[nodiscard]] constexpr size_t size() noexcept { return {}; }
template<class T, size_t N>
[[nodiscard]] constexpr size_t size(const T (&)[N]) noexcept { return N; }

template<class T, class ...Args>
[[nodiscard]] size_t size(const T &head, const Args &...args) noexcept { return size(head) + size(args...); }

template<class T> std::wstring str_to_wide(T str) { return std::to_wstring(str); }

template<> inline std::wstring str_to_wide<const std::wstring &>(const std::wstring &str) { return str; }
template<> inline std::wstring str_to_wide<const std::wstring_view &>(const std::wstring_view &str) { return std::wstring{ str }; }
template<> inline std::wstring str_to_wide<const wchar_t *>(const wchar_t *str) { return std::wstring{ str }; }
template<> inline std::wstring str_to_wide<wchar_t>(const wchar_t c) { return std::wstring{ c }; }

template<> inline std::wstring str_to_wide<const char *>(const char *str) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.from_bytes(str);
}
template<> inline std::wstring str_to_wide<const std::string &>(const std::string &str) {
	return str_to_wide(str.c_str());
}
template<> inline std::wstring str_to_wide<const std::string_view &>(const std::string_view &str) {
	return str_to_wide(str.data());
}
template<> inline std::wstring str_to_wide<char>(const char c) {
	return std::wstring{ static_cast<wchar_t>(c) };
}

inline std::string wide_to_str(const std::wstring &wstr) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

template<class... Args>
[[nodiscard]] std::string concat(const Args &...args) noexcept {
	std::string result;
	result.reserve(size(args...));
	(result.append(args), ...);
	return result;
}

template<class ...Args>
[[nodiscard]] std::wstring concat_wide(const Args &...args) noexcept {
	std::wstring result;
	result.reserve(size(args...));
	(result.append(str_to_wide(args)), ...);
	return result;
}

template<class T>
[[nodiscard]] core::usize count_frequency(
		const std::basic_string_view<T> &str, const std::basic_string_view<T> &pattern) noexcept {
	const auto M{ size(pattern) };
	const auto N{ size(str) };
	if (M > N) return 0;

	size_t res{};

	for (size_t i{}; i <= N - M; ++i) {
		size_t j;
		for (j = 0; j < M; ++j) {
			if (str[i + j] != pattern[j]) break;
		}

		if (j == M) res++;
	}
	return static_cast<core::usize>(res);
}
template<class T>
[[nodiscard]] core::usize count_frequency(const std::basic_string_view<T> &str, const T pattern) noexcept {
	return std::count(std::begin(str), std::end(str), pattern);
}

template<class T>
[[nodiscard]] std::vector<std::basic_string<T>> split(
		const std::basic_string_view<T> &str, const std::basic_string_view<T> &delimiter) noexcept {
	const auto delimiters_count{ count_frequency(str, delimiter) + 1 };
	if (delimiters_count == 1) return { std::basic_string<T>{ str } };

	std::vector<std::basic_string<T>> tokens;
	tokens.reserve(static_cast<size_t>(delimiters_count));
	size_t prev = 0;
	size_t pos = 0;
	do {
		pos = str.find_first_of(delimiter, prev);
		if (pos == str.npos) pos = str.length();

		if (auto token{ str.substr(prev, pos - prev) }; !token.empty()) {
			tokens.emplace_back(std::move(token));
		}
		prev = pos + 1;
	} while (pos < str.length() && prev < str.length());

	return tokens;
}

template<class T>
[[nodiscard]] std::vector<std::basic_string<T>> split(
		const std::basic_string_view<T> &str, const T &delimiter) noexcept {

	const auto delimiters_count{ count_frequency(str, delimiter) + 1};
	if (delimiters_count == 1) return { std::basic_string<T>{ str } };

	std::vector<std::basic_string<T>> tokens;
	tokens.reserve(static_cast<size_t>(delimiters_count));
	size_t prev = 0;
	size_t pos = 0;
	do {
		pos = str.find_first_of(delimiter, prev);
		if (pos == str.npos) pos = str.length();

		if (auto token{ str.substr(prev, pos - prev) }; !token.empty()) {
			tokens.emplace_back(std::move(token));
		}
		prev = pos + 1;
	} while (pos < str.length() && prev < str.length());

	return tokens;
}

template<class T>
[[nodiscard]] auto dir_and_extension(const std::basic_string_view<T> &name) noexcept
		-> std::pair<std::remove_all_extents_t<decltype(name)>, std::remove_all_extents_t<decltype(name)>> {
	using view_type = std::remove_all_extents_t<decltype(name)>;

	if (const auto dot_pos{ name.find_last_of('.') }; dot_pos != std::string_view::npos) {
		return std::make_pair(name.substr(0, dot_pos), name.substr(dot_pos));
	}
	return std::make_pair(name, view_type{});
}
template<class T>
[[nodiscard]] auto dir_and_extension(const std::basic_string<T> &name) noexcept
		-> std::pair<std::basic_string_view<T>, std::basic_string_view<T>> {
	return dir_and_extension(std::basic_string_view<T>{ name });
}

template<class T>
[[nodiscard]] std::basic_string<T> strip(const std::basic_string_view<T> str) noexcept {
	static constexpr auto whitespaces{ whitespaces_v<T> };

	const auto start{ str.find_first_not_of(whitespaces) };
	const auto end{ str.find_last_not_of(whitespaces) };
	return std::basic_string<T>{ str.substr(start, start - end) };
}

template<class T, size_t N>
[[nodiscard]] std::basic_string<T> strip(const T (&str)[N]) {
	return strip(std::basic_string_view<T>{ str, N });
}


} // namespace golxzn::core::utils
