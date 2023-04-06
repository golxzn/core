#include <spdlog/spdlog.h>

#include "golxzn/core/types/image.hpp"

#include "golxzn/core/utils/algorithm.hpp"
#include "golxzn/core/utils/traits.hpp"

namespace golxzn::core::types {

image::image(const u32 width, const u32 height, const channel img_channel, const color fill_color) noexcept
	: m_width{ width }, m_height{ height }, m_channel{ img_channel } {
	fill(fill_color);
}

image::image(const u32 width, const u32 height, const channel img_channel, const bytes &data) noexcept
	: m_width{ width }, m_height{ height }, m_channel{ img_channel }, m_data{ data } {
	if (m_data.size() < bytes_count()) {
		spdlog::warn(
			"[{}] The data is too small for image {}x{}."
			"The bytes length is {}, but has to be {} at least",
			class_name, m_width, m_height, m_data.size(), bytes_count()
		);
	}
}

image::image(const u32 width, const u32 height, const channel img_channel, bytes &&data) noexcept
	: m_width{ width }, m_height{ height }, m_channel{ img_channel }, m_data{ std::move(data) } {
	if (m_data.size() < bytes_count()) {
		spdlog::warn(
			"[{}] The data is too small for image {}x{}."
			"The bytes length is {}, but has to be {} at least",
			class_name, m_width, m_height, m_data.size(), bytes_count()
		);
	}
}

u32 image::width() const noexcept {
	return m_width;
}
u32 image::height() const noexcept {
	return m_height;
}
u32 image::stride() const noexcept {
	return m_width * color_count();
}
u32 image::color_count() const noexcept {
	return channel_count(m_channel);
}
u32 image::bytes_count() const noexcept {
	return m_width * m_height * color_count();
}
const bytes &image::raw() const noexcept {
	return m_data;
}
bool image::empty() const noexcept {
	return m_data.empty();
}

color image::pixel(const u32 x, const u32 y) const noexcept {
	if (x > m_width || y > m_height) {
		spdlog::warn("[{}] {} is out of range", class_name, x > m_width ? "x" : "y");
		return default_fill_color;
	}
	const auto data{ m_data.data() + ((y * m_width + x) * color_count()) };
	switch (m_channel) {
		case channel::grey: return color{ data[0], data[0], data[0], static_cast<u8>(0xFF) };
		case channel::grey_alpha: return color{ data[0], data[0], data[0], data[1] };
		case channel::rgb: return color{ data[0], data[1], data[2], static_cast<u8>(0xFF) };
		case channel::rgba: return color{ data[0], data[1], data[2], data[3] };
	}
	return default_fill_color;
}

void image::set_pixel(const u32 x, const u32 y, const color color) noexcept {
	if (x > m_width || y > m_height) {
		spdlog::warn("[{}] {} is out of range", class_name, x > m_width ? "x" : "y");
		return;
	}
	const auto data{ m_data.data() + ((y * m_width + x) * color_count()) };
	const auto raw_color{ color.raw() };
	switch (m_channel) {
		case channel::grey: data[0] = raw_color[0]; break;
		case channel::grey_alpha: data[0] = raw_color[0]; data[1] = raw_color[3]; break;
		case channel::rgb: std::copy_n(raw_color, color_count() - 1, data); break;
		case channel::rgba: std::copy_n(raw_color, color_count(), data); break;
	}
}

void image::replace(const color src_color, const color dst_color) noexcept {
	if (m_data.empty()) return;

	const auto color_cnt{ color_count() };
	const auto data_ptr_end{ m_data.data() + bytes_count() };
	for (auto data_ptr{ m_data.data() }; data_ptr <= data_ptr_end; data_ptr += color_cnt) {
		if (std::equal(data_ptr, data_ptr + color_cnt, src_color.raw())) {
			std::copy_n(dst_color.raw(), color_cnt, data_ptr);
		}
	}
}

void image::mask(const color mask_color) noexcept {
	replace(mask_color, color{ 0x00000000 });
}

void image::fill(const color fill_color) noexcept {
	const auto num_of_pixels{ bytes_count() };
	m_data.resize(num_of_pixels);
	const auto raw_color{ fill_color.raw() };
	const auto color_cnt{ color_count() };
	const auto data_ptr_end{ m_data.data() + bytes_count() };
	for (auto data_ptr{ m_data.data() }; data_ptr <= data_ptr_end;) {
		data_ptr = std::copy_n(raw_color, color_cnt, data_ptr);
	}
}

void image::flip(const flip_direction dir) noexcept {
	if (m_data.empty()) {
		return;
	}

	const auto color_cnt{ color_count() };
	auto data_ptr{ m_data.data() };
	if (flip_direction::both == dir) {
		const auto data_ptr_end{ m_data.data() + bytes_count() };
		auto back_iter{ data_ptr_end - color_cnt };
		for (; data_ptr < back_iter; data_ptr += color_cnt, back_iter -= color_cnt) {
			std::swap_ranges(data_ptr, data_ptr + color_cnt, back_iter);
		}
	} else if (flip_direction::vertical == dir) {
		for (auto x{ 0u }; x < m_width; ++x) {
			for (auto y{ 0u }; y < m_height / 2; ++y) {
				auto insert_position{ data_ptr + (y * m_width + x) * color_cnt };
				auto back_position{ data_ptr + ((m_height - y - 1) * m_width + x) * color_cnt };
				std::swap_ranges(insert_position, insert_position + color_cnt, back_position);
			}
		}
	} else if (flip_direction::horizontal == dir) {
		for (auto y{ 0u }; y < m_height; ++y) {
			for (auto x{ 0u }; x < m_width / 2; ++x) {
				auto insert_position{ data_ptr + (y * m_width + x) * color_cnt };
				auto back_position{ data_ptr + (y * m_width + (m_width - x - 1)) * color_cnt };
				std::swap_ranges(insert_position, insert_position + color_cnt, back_position);
			}
		}
	}
}

// void image::copy(const point2<i32> &pos, const ref &source, const rect<u32> &source_rect, overlap_policy policy) {
// 	if (source != nullptr) {
// 		copy(pos, *source, source_rect, policy);
// 	}
// }

// void image::copy(const point2<i32> &pos, const image &source, const rect<u32> &source_rect, overlap_policy policy) {
// 	if (empty() || source.empty()) {
// 		const static auto b2str{ [](auto value) { return value ? "empty" : "not empty"; } };
// 		spdlog::warn("[{}] Cannot copy empty images. (this: {}, source: {})",
// 			class_name, b2str(empty()), b2str(source.empty()));
// 		return;
// 	}

// 	auto position{ pos };
// 	rect<u32> src_frame{ reduce_rect(position, source, source_rect, policy) };
// 	rect<u32> dest_frame{ 0, 0, m_width, m_height };

// 	auto result_frame{ dest_frame.overlap(rect<u32>{
// 		static_cast<u32>(position.at(0) > 0 ? position.at(0) : 0),
// 		static_cast<u32>(position.at(1) > 0 ? position.at(0) : 0),
// 		src_frame.width, src_frame.height
// 	}) };
// 	if (result_frame == src_frame) {
// 		m_data = source.m_data;
// 		m_width = source.m_width;
// 		m_height = source.m_height;
// 		return;
// 	}

// 	if (policy == overlap_policy::expand_target && (pos.at(0) < 0 || pos.at(1) < 0  || dest_frame < result_frame)) {
// 		const auto left_padding{ pos.at(0) < 0 ? std::abs(pos.at(0)) : 0 };
// 		const auto top_padding{ pos.at(1) < 0 ? std::abs(pos.at(1)) : 0 };
// 		const auto right_padding{ result_frame.width - dest_frame.width };
// 		const auto bottom_padding{ result_frame.height - dest_frame.height };
// 		expand(left_padding, top_padding, right_padding, bottom_padding);
// 		dest_frame = rect<u32>{ 0, 0, m_width, m_height };
// 		position = {};
// 		src_frame = reduce_rect(position, source, source_rect, overlap_policy::discard_source);
// 		result_frame = dest_frame.overlap(src_frame);
// 	}

// 	if (dest_frame == result_frame) {
// 		const auto source_line_start_offset{ src_frame.left() };
// 		const auto source_line_end_offset{ source.width() - src_frame.right() };

// 		auto target_ptr{ colors_ptr() };
// 		auto source_ptr{ source.colors_ptr() + src_frame.top() * source.width() };

// 		target_ptr += position.at(1) * m_width;
// 		for (u32 row{}; row < src_frame.height; ++row) {
// 			target_ptr += position.at(0);
// 			source_ptr += source_line_start_offset;
// 			target_ptr = std::copy_n(source_ptr, src_frame.width, target_ptr);
// 			source_ptr += source_line_end_offset + src_frame.width;
// 			target_ptr += m_width - (position.at(0) + src_frame.width);
// 		}
// 		return;
// 	}
// }

void image::crop(const u32 x, const u32 y, const u32 width, const u32 height) noexcept {
	if (empty()) {
		spdlog::warn("[{}] Cannot crop empty image", class_name);
		return;
	}

	if (x >= m_width || y >= m_height) {
		spdlog::warn("[{}] Crop rect '[{}, {}, {}, {}]' is out of range",
			class_name, x, y, width, height);
		return;
	}

	const auto w{ width  == 0 ? m_width - x  : std::min(width, m_width - x) };
	const auto h{ height == 0 ? m_height - y : std::min(height, m_height - y) };

	if (traits::all_from(0, x, y) && w == m_width) {
		if (h == m_height) return; // Nothing to do.
		m_height = h;
		m_data.resize(bytes_count());
		return;
	}

	const auto color_cnt{ color_count() };
	bytes data(w * h * color_cnt);

	const auto line_bytes_count{ w * color_cnt };
	const auto line_start_offset{ x * color_cnt };
	const auto line_end_offset{ (m_width - (x + w)) * color_cnt };

	auto data_ptr{ m_data.data() + y * m_width * color_cnt };
	auto new_data_ptr{ data.data() };
	for (u32 row{}; row < h; ++row) {
		data_ptr += line_start_offset;
		new_data_ptr = std::copy_n(data_ptr, line_bytes_count, new_data_ptr);
		data_ptr += line_bytes_count + line_end_offset;
	}

	m_data = std::move(data);
	m_width = w;
	m_height = h;
}

void image::crop(const rect<u32> &crop_rect) noexcept {
	crop(crop_rect.x, crop_rect.y, crop_rect.width, crop_rect.height);
}

void image::expand(const u32 left, const u32 up, const u32 right, const u32 down, const color fill_color) noexcept {
	if (traits::all_from(0, left, up, right, down)) return;

	if (empty()) {
		m_width = left + right;
		m_height = up + down;
		fill(fill_color);
		return;
	}

	const auto color_cnt{ color_count() };
	const u32 bytes_per_width{ m_width * color_cnt };
	if (traits::all_from(0, left, up, right)) {
		const u32 new_height{ m_height + down };
		m_data.resize(new_height * bytes_per_width);
		auto data_ptr{ m_data.data() + m_height * bytes_per_width };
		utils::fill_range_n(data_ptr, down * bytes_per_width, fill_color.raw(), color_cnt);

		m_height = new_height;
		return;
	}

	auto data_ptr{ m_data.data() };

	const auto new_width{ left + m_width + right };
	const auto new_height{ up + m_height + down };
	bytes data(new_width * new_height * color_cnt);

	const auto left_offset{ left * color_cnt };
	const auto right_offset{ right * color_cnt };

	auto new_data_ptr{ data.data() };
	new_data_ptr = utils::fill_range_n(new_data_ptr, new_width * up * color_cnt, fill_color.raw(), color_cnt);
	for (u32 row{}; row < m_height; ++row) {
		new_data_ptr = utils::fill_range_n(new_data_ptr, left_offset, fill_color.raw(), color_cnt);
		new_data_ptr = std::copy_n(data_ptr + row * bytes_per_width, bytes_per_width, new_data_ptr);
		new_data_ptr = utils::fill_range_n(new_data_ptr, right_offset, fill_color.raw(), color_cnt);
	}
	utils::fill_range_n(new_data_ptr, new_width * down * color_cnt, fill_color.raw(), color_cnt);

	m_data = std::move(data);
	m_width = new_width;
	m_height = new_height;

}

void image::expand(const rect<u32> &expand_rect, const color fill_color) noexcept {
	return expand(expand_rect.x, expand_rect.y, expand_rect.width, expand_rect.height, fill_color);
}

// void image::resize(const u32 width, const u32 height, const color fill_color) {
// 	if (width == m_width && height == m_height) return;
// 	if (width == 0 || height == 0) {
// 		clean();
// 		return;
// 	}

// 	enum class method{ expand, crop, no_change };
// 	static const auto select_method{ [](const u32 origin, const u32 new_value) {
// 		if (origin == new_value) return method::no_change;
// 		if (origin > new_value) return method::crop;
// 		return method::expand;
// 	} };

// 	const auto width_method{ select_method(m_width, width) };
// 	const auto height_method{ select_method(m_height, height) };

// 	// if (width_method == method::expand && height_method == method::expand) {
// 	// 	expand(width, height, fill_color);
// 	// } else if (width_method == method::crop && height_method == method::crop) {
// 	// 	crop(width, height);
// 	// } else if (width_method == method::expand && height_method == method::crop) {
// 	// 	expand(width, m_height, fill_color);
// 	// 	crop(width, height);
// 	// } else if (width_method == method::crop && height_method == method::expand) {
// 	// 	crop(m_width, height);
// 	// 	expand(m_width, height, fill_color);
// 	// } else {
// 	// 	spdlog::warn("[{}] Unknown method", class_name);
// 	// }
// }

void image::clean() noexcept {
	m_data.clear();
	m_width = 0;
	m_height = 0;
}

bool image::operator==(const image &other) const noexcept {
	return m_width == other.m_width && m_height == other.m_height && m_data == other.m_data;
}
bool image::operator!=(const image &other) const noexcept {
	return !(*this == other);
}

bool image::operator<(const image &other) const noexcept {
	return bytes_count() < other.bytes_count() || m_data < other.m_data;
}
bool image::operator>(const image &other) const noexcept {
	return bytes_count() > other.bytes_count() || m_data > other.m_data;
}
bool image::operator<=(const image &other) const noexcept {
	return !(other > *this);
}
bool image::operator>=(const image &other) const noexcept {
	return !(other < *this);
}

// rect<u32> image::reduce_rect(point2<i32> &pos, const image &source,
// 	const rect<u32> &source_rect, overlap_policy policy) const noexcept {

// 	rect<u32> frame{
// 		source_rect.empty()
// 			? rect<u32>{ 0, 0, source.m_width, source.m_height }
// 			: source_rect
// 	};
// 	if (frame.right() > source.m_width)   frame.width = source.m_width - frame.x;
// 	if (frame.bottom() > source.m_height) frame.height = source.m_height - frame.y;

// 	if (overlap_policy::discard_source == policy) {
// 		if (const auto abs_x{ std::abs(pos.at(0)) }; pos.at(0) < 0) {
// 			pos.at(0) = 0;
// 			frame.x += abs_x;
// 			frame.width -= abs_x;
// 		}
// 		if (const auto abs_y{ std::abs(pos.at(1)) }; pos.at(1) < 0) {
// 			pos.at(1) = 0;
// 			frame.y += abs_y;
// 			frame.height -= abs_y;
// 		}

// 		if (const auto delta{ m_width - pos.at(0) }; frame.width > delta)   frame.width = delta;
// 		if (const auto delta{ m_height - pos.at(1) }; frame.height > delta) frame.height = delta;
// 	}
// 	return frame;
// }


} // namespace golxzn::core::types
