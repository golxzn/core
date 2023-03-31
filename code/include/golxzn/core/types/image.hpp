#pragma once

#include <iterator>
#include <bitset>
#include "golxzn/core/utils/error.hpp"
#include "golxzn/core/types.hpp"
#include "golxzn/core/types/color.hpp"
#include "golxzn/core/types/rectangle.hpp"

namespace golxzn::core::types {

class image {
	static constexpr std::string_view class_name{ "golxzn::core::types::image" };
public:
	using ref = sptr<image>;

	static constexpr color default_fill_color{ 0xFFFFFFFF };
	static constexpr u32 color_count{ 4 };

	enum class flip_direction : u8 {
		both,
		horizontal,
		vertical
	};

	GOLXZN_DEFAULT_CLASS(image);

	image(const u32 width, const u32 height, const color fill_color = default_fill_color) noexcept;
	image(const u32 width, const u32 height, const bytes &data) noexcept;
	image(const u32 width, const u32 height, bytes &&data) noexcept;

	template<class InputIterator>
	image(const u32 width, const u32 height, InputIterator _begin, InputIterator _end);

	[[nodiscard]] u32 width() const noexcept;
	[[nodiscard]] u32 height() const noexcept;
	[[nodiscard]] u32 stride() const noexcept;
	[[nodiscard]] u32 bytes_count() const noexcept;
	[[nodiscard]] const bytes &raw() const noexcept;
	[[nodiscard]] bool empty() const noexcept;

	[[nodiscard]] color &pixel(const u32 x, const u32 y);
	[[nodiscard]] color pixel(const u32 x, const u32 y) const noexcept;

	// enum class copy_policy{ discard, resize };

	// void copy(const point2<u32> &pos, const image &other,
	// 	const rect<u32> &source_rect = {}, copy_policy policy = copy_policy::discard);

	void crop(const u32 x, const u32 y, const u32 width, const u32 height) noexcept;
	void crop(const rect<u32> &rect) noexcept;
	void expand(const u32 left, const u32 up, const u32 right, const u32 down, const color fill_color = default_fill_color) noexcept;
	void expand(const rect<u32> &rect, const color fill_color = default_fill_color) noexcept;
	// void resize(const u32 width, const u32 height, const color fill_color = default_fill_color) noexcept;
	void replace(const color from, const color to) noexcept;
	void mask(const color mask_color) noexcept;
	void fill(const color fill_color = default_fill_color) noexcept;
	void flip(const flip_direction direction) noexcept;
	void clean() noexcept;

	[[nodiscard]] bool operator==(const image &other) const noexcept;
	[[nodiscard]] bool operator!=(const image &other) const noexcept;
	[[nodiscard]] bool operator<(const image &other) const noexcept;
	[[nodiscard]] bool operator>(const image &other) const noexcept;
	[[nodiscard]] bool operator<=(const image &other) const noexcept;
	[[nodiscard]] bool operator>=(const image &other) const noexcept;

private:
	u32 m_width{};
	u32 m_height{};
	bytes m_data{};

	// rect<u32> reduce_rect(const point2<u32> &pos, const image &other, const rect<u32> &source_rect,
	// 	copy_policy policy) const noexcept;

	const color *const colors_ptr() const noexcept;
	color *colors_ptr() noexcept;
};

template<class InputIterator>
image::image(const u32 width, const u32 height, InputIterator _begin, InputIterator _end)
	: m_width{ width }, m_height{ height } {
	const auto num_of_pixels{ bytes_count() };
	if (num_of_pixels > std::distance(_begin, _end)) {
		clean();
		utils::error::runtime_error("[golxzn::core::types::image] Not enough data");
	} else {
		m_data.reserve(num_of_pixels);
		m_data.assign(_begin, _end);
	}
}

} // namespace golxzn::core::types
