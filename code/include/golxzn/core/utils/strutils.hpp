#pragma once

#include <locale>
#include <codecvt>
#include <golxzn/core/aliases.hpp>
#include <golxzn/core/utils/traits.hpp>

namespace golxzn::core::utils {

[[nodiscard]] inline size_t size(const char *s) noexcept { return std::strlen(s); }
[[nodiscard]] inline size_t size(const wchar_t *s) noexcept { return std::wcslen(s); }
[[nodiscard]] inline size_t size(const std::string &s) noexcept { return s.size(); }
[[nodiscard]] inline size_t size(const std::wstring &s) noexcept { return s.size(); }
[[nodiscard]] constexpr size_t size(const std::string_view &s) noexcept { return s.size(); }
[[nodiscard]] constexpr size_t size(const std::wstring_view &s) noexcept { return s.size(); }
[[nodiscard]] constexpr size_t size(const char &) noexcept { return sizeof(char); }
[[nodiscard]] constexpr size_t size(const wchar_t &) noexcept { return sizeof(wchar_t); }
[[nodiscard]] constexpr size_t size() noexcept { return {}; }

template<class ...Args>
[[nodiscard]] size_t size(const Args &...args) noexcept { return ((size(args) - 1) + ...) + 1; }
template<class ...Args>
[[nodiscard]] size_t size(const char *s, const Args &...args) noexcept { return std::strlen(s) + size(args...); }
template<class ...Args>
[[nodiscard]] size_t size(const wchar_t *s, const Args &...args) noexcept { return std::wcslen(s) + size(args...); }
template<class ...Args>
[[nodiscard]] size_t size(const std::string &s, const Args &...args) noexcept { return s.size() + size(args...); }
template<class ...Args>
[[nodiscard]] size_t size(const std::wstring &s, const Args &...args) noexcept { return s.size() + size(args...); }
template<class ...Args>
[[nodiscard]] size_t size(const std::string_view &s, const Args &...args) noexcept { return s.size() + size(args...); }
template<class ...Args>
[[nodiscard]] size_t size(const std::wstring_view &s, const Args &...args) noexcept { return s.size() + size(args...); }
template<class ...Args>
[[nodiscard]] size_t size(const char &c, const Args &...args) noexcept { return sizeof(c) + size(args...); }
template<class ...Args>
[[nodiscard]] size_t size(const wchar_t &c, const Args &...args) noexcept { return sizeof(c) + size(args...); }

template<class T> std::wstring str_to_wide(T str);

template<> std::wstring str_to_wide<const std::wstring &>(const std::wstring &str) { return str; }
template<> std::wstring str_to_wide<const std::wstring_view &>(const std::wstring_view &str) { return std::wstring{ str }; }
template<> std::wstring str_to_wide<const wchar_t *>(const wchar_t *str) { return std::wstring{ str }; }
template<> std::wstring str_to_wide<wchar_t>(const wchar_t c) { return std::wstring{ c }; }

template<> std::wstring str_to_wide<const char *>(const char *str) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.from_bytes(str);
}
template<> std::wstring str_to_wide<const std::string &>(const std::string &str) {
	return str_to_wide(str.c_str());
}
template<> std::wstring str_to_wide<const std::string_view &>(const std::string_view &str) {
	return str_to_wide(str.data());
}
template<> std::wstring str_to_wide<char>(const char c) {
	return std::wstring{ static_cast<wchar_t>(c) };
}

std::string wide_to_str(const std::wstring &wstr) {
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
	(result.append(str_to_widt(args)), ...);
	return result;
}

} // namespace golxzn::core::utils
