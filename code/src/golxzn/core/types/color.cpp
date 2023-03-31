#include <algorithm>
#include <cctype>

#include "golxzn/core/types/color.hpp"
#include "golxzn/core/utils/error.hpp"

namespace golxzn::core {

u8 *color::raw() noexcept {
	return reinterpret_cast<u8 *>(this);
}
const u8 *const color::raw() const noexcept {
	return reinterpret_cast<const u8 *const >(this);
}

u8 *color::begin() noexcept { return raw(); }
u8 *color::end() noexcept { return raw() + 4; }
const u8 *const color::begin() const noexcept { return raw(); }
const u8 *const color::end() const noexcept { return raw() + 4; }


bool validate_hex_color(const std::string_view str) {
	if (str.empty() || str.at(0) != '#') return false;

	const auto hex{ str.substr(1) };
	if (hex.size() != 6 && hex.size() != 8) {
		return false;
	}
	return std::all_of(std::begin(hex), std::end(hex), [](const auto c) {
		return std::isxdigit(c);
	});
}

color make_color(const std::string_view str) {
	if (str.empty()) return {};
	if (str[0] == '#') {
		if (!validate_hex_color(str)) {
			using namespace std::string_literals;
			core::utils::error::runtime_error("invalid color hex: "s + std::string{ str });
			return {};
		}
		const auto hex{ str.substr(1) };
		const auto clr{ std::strtoul(hex.data(), nullptr, 16) };
		if (hex.size() == 6) {
			return color{ clr | 0xFF000000 };
		}
		return color{ clr };
	}

	using namespace std::string_view_literals;

	static const umap<std::string_view, color> color_map{
		{ "black"sv,       color{   0,   0,   0 } },
		{ "white"sv,       color{ 255, 255, 255 } },
		{ "red"sv,         color{ 255,   0,   0 } },
		{ "green"sv,       color{   0, 255,   0 } },
		{ "blue"sv,        color{   0,   0, 255 } },
		{ "yellow"sv,      color{ 255, 255,   0 } },
		{ "cyan"sv,        color{   0, 255, 255 } },
		{ "purple"sv,      color{ 255,   0, 255 } },
		{ "orange"sv,      color{ 255, 127,   0 } },
		{ "pink"sv,        color{ 255, 192, 203 } },
		{ "brown"sv,       color{ 165,  42,  42 } },
		{ "gray"sv,        color{ 128, 128, 128 } },
		{ "navy"sv,        color{   0,   0, 128 } },
		{ "magenta"sv,     color{ 255,   0, 255 } },
		{ "dark_green"sv,  color{   0, 100,   0 } },
		{ "dark_blue"sv,   color{   0,   0, 139 } },
		{ "dark_red"sv,    color{ 139,   0,   0 } },
		{ "dark_gray"sv,   color{ 169, 169, 169 } },
		{ "dark_brown"sv,  color{ 101,  67,  33 } },
		{ "gold"sv,        color{ 255, 215,   0 } },
		{ "silver"sv,      color{ 192, 192, 192 } },
		{ "light_gray"sv,  color{ 211, 211, 211 } },
		{ "light_blue"sv,  color{ 173, 216, 230 } },
		{ "light_brown"sv, color{ 210, 180, 140 } },
		{ "light_green"sv, color{ 144, 238, 144 } }
	};
	if (auto found{ color_map.find(str) }; found != std::end(color_map)) {
		return found->second;
	}
	return {};
}

namespace color_literals {

color operator""_clr(const char *str, const size_t size) {
	return make_color(std::string_view{ str, size });
}

} // namespace color_literals


} // namespace golxzn::core
