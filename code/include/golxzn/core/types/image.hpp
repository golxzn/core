#pragma once

#include <iterator>
#include <bitset>
#include "golxzn/core/export.hpp"
#include "golxzn/core/utils/error.hpp"
#include "golxzn/core/aliases.hpp"
#include "golxzn/core/types/color.hpp"
#include "golxzn/core/types/rectangle.hpp"

namespace golxzn::core::types {

class GOLXZN_EXPORT image {
	static constexpr std::string_view class_name{ "golxzn::core::types::image" };
public:
	using ref = sptr<image>;
	using position = glm::vec<2, i32>;

	static constexpr color default_fill_color{ 0xFFFFFFFF };

	enum class channel : u32 {
		invalid    = 0,
		grey       = 1,
		grey_alpha = 2,
		rgb        = 3,
		rgba       = 4,

		gray = grey,
		gray_alpha = grey_alpha,
	};

	enum class flip_direction : u8 {
		both,
		horizontal,
		vertical
	};

	GOLXZN_DEFAULT_CLASS(image);

	image(const u32 width, const u32 height, const channel img_channel, const color fill_color = default_fill_color) noexcept;
	image(const u32 width, const u32 height, const channel img_channel, const bytes &data) noexcept;
	image(const u32 width, const u32 height, const channel img_channel, bytes &&data) noexcept;

	template<class InputIterator>
	image(const u32 width, const u32 height, const channel img_channel, InputIterator _begin, InputIterator _end);

	[[nodiscard]] u32 width() const noexcept;
	[[nodiscard]] u32 height() const noexcept;
	[[nodiscard]] u32 stride() const noexcept;
	[[nodiscard]] u32 color_count() const noexcept;
	[[nodiscard]] u32 bytes_count() const noexcept;
	[[nodiscard]] channel get_channel() const noexcept;
	[[nodiscard]] const bytes &raw() const noexcept;
	[[nodiscard]] bool empty() const noexcept;

	[[nodiscard]] color pixel(const u32 x, const u32 y) const noexcept;
	void set_pixel(const u32 x, const u32 y, const color color) noexcept;

	enum class overlap_policy{
		discard_target,
		expand_source
	};

	void copy(const position &pos, const ref &target, const rect<u32> &target_rect = {},
		const bool apply_alpha = true, overlap_policy policy = overlap_policy::discard_target);
	void copy(const position &pos, const image &target, const rect<u32> &target_rect = {},
		const bool apply_alpha = true, overlap_policy policy = overlap_policy::discard_target);

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

	static constexpr u32 channel_count(const channel channel) noexcept {
		return static_cast<u32>(channel);
	}
	static constexpr channel to_channel(const u32 channel_count) noexcept {
		return static_cast<channel>(channel_count);
	}
private:
	u32 m_width{};
	u32 m_height{};
	channel m_channel{ channel::invalid };
	bytes m_data{};

};

template<class InputIterator>
image::image(const u32 width, const u32 height, const channel ch, InputIterator _begin, InputIterator _end)
	: m_width{ width }, m_height{ height }, m_channel{ ch } {
	const auto num_of_pixels{ bytes_count() };
	if (num_of_pixels > std::distance(_begin, _end)) {
		clean();
		utils::error::runtime_error("[golxzn::core::types::image] Not enough data");
	} else {
		m_data.reserve(num_of_pixels);
		m_data.assign(_begin, _end);
	}
}

using colour = color;

} // namespace golxzn::core::types
