#pragma once

#include <glm/vec4.hpp>
#include <string_view>
#include "golxzn/core/export.hpp"
#include "golxzn/core/utils/traits.hpp"
#include "golxzn/core/types.hpp"

namespace golxzn::core {

struct GOLXZN_EXPORT color {
	static constexpr u8 max_alpha{ static_cast<u8>(255) };
	static constexpr f16 max_alpha_f{ 255.0f };

	constexpr color() noexcept = default;
	constexpr color(const color &) noexcept = default;
	constexpr color(color &&) noexcept = default;
	constexpr color &operator=(const color &) noexcept = default;
	constexpr color &operator=(color &&) noexcept = default;

	template<class T, std::enable_if_t<std::is_integral_v<T>, bool> = false>
	constexpr color(const T red, const T green, const T blue, const T alpha = max_alpha) noexcept
		: r{ static_cast<u8>(red) } , g{ static_cast<u8>(green) }
		, b{ static_cast<u8>(blue) } , a{ static_cast<u8>(alpha) } {}

	template<class T, std::enable_if_t<std::is_floating_point_v<T>, bool> = false>
	constexpr color(const T red, const T green, const T blue, const T alpha = 1.0) noexcept
		: r{ static_cast<u8>(red * max_alpha_f) }, g{ static_cast<u8>(green * max_alpha_f) }
		, b{ static_cast<u8>(blue * max_alpha_f) }, a{ static_cast<u8>(alpha * max_alpha_f) } {}

	constexpr explicit color(const u32 color) noexcept
		: r{ static_cast<u8>((color >> 16) & 0xFF) }
		, g{ static_cast<u8>((color >> 8) & 0xFF) }
		, b{ static_cast<u8>(color & 0xFF) }
		, a{ static_cast<u8>((color >> 24) & 0xFF) } {}

	template<class T, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, bool> = false>
	constexpr explicit color(const glm::vec<4, T> &vec) noexcept
		: color{ vec.r, vec.g, vec.b, vec.a } {}

	u8 *raw() noexcept;
	const u8 *const raw() const noexcept;

	constexpr bool operator==(const color &other) const noexcept {
		return r == other.r && g == other.g && b == other.b && a == other.a;
	}
	constexpr bool operator!=(const color &other) const noexcept {
		return !this->operator==(other);
	}

	template<class T, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, bool> = false>
	constexpr bool operator==(const glm::vec<4, T> &vec) const noexcept {
		return this->operator==(color{ vec });
	}
	template<class T, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, bool> = false>
	constexpr bool operator!=(const glm::vec<4, T> &vec) const noexcept {
		return this->operator!=(color{ vec });
	}

	template<class T, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, bool> = false>
	constexpr operator glm::vec<4, T>() const noexcept {
		return glm::vec<4, T>{
			static_cast<T>(r),
			static_cast<T>(g),
			static_cast<T>(b),
			static_cast<T>(a)
		};
	}

	union { u8 red{ max_alpha };   u8 r; };
	union { u8 green{ max_alpha }; u8 g; };
	union { u8 blue{ max_alpha };  u8 b; };
	union { u8 alpha{ max_alpha }; u8 a; };

	u8 *begin() noexcept;
	u8 *end() noexcept;
	const u8 *const begin() const noexcept;
	const u8 *const end() const noexcept;
};

bool validate_hex_color(const std::string_view str);

color make_color(const std::string_view str);

namespace color_literals {

[[nodiscard]] color operator""_clr(const char *str, const size_t size);

} // namespace color_literals

} // namespace golxzn::core
