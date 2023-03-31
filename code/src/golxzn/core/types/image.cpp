#include <spdlog/spdlog.h>

#include "golxzn/core/types/image.hpp"

namespace golxzn::core::types {

image::image(const u32 width, const u32 height, const color fill_color) noexcept
	: m_width{ width }, m_height{ height } {
	fill(fill_color);
}

image::image(const u32 width, const u32 height, const bytes &data) noexcept
	: m_width{ width }, m_height{ height }, m_data{ data } {
	if (m_data.size() < bytes_count()) {
		spdlog::warn(
			"[{}] The data is too small for image {}x{}."
			"The bytes length is {}, but has to be {} at least",
			class_name, m_width, m_height, m_data.size(), bytes_count()
		);
	}
}

image::image(const u32 width, const u32 height, bytes &&data) noexcept
	: m_width{ width }, m_height{ height }, m_data{ std::move(data) } {
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
	return m_width * color_count;
}
u32 image::bytes_count() const noexcept {
	return m_width * m_height * color_count;
}
const bytes &image::raw() const noexcept {
	return m_data;
}
bool image::empty() const noexcept {
	return m_data.empty();
}

color &image::pixel(const u32 x, const u32 y) {
	if (x > m_width || y > m_height) {
		utils::error::out_of_range("[golxzn::core::types::image] pixel: out of range!");
	}
	return colors_ptr()[y * m_width + x];
}

color image::pixel(const u32 x, const u32 y) const noexcept {
	if (x > m_width || y > m_height) {
		spdlog::warn("[{}] {} is out of range", class_name, x > m_width ? "x" : "y");
		return default_fill_color;
	}
	if (m_data.empty()) { // Actually, it will never be empty.
		spdlog::warn("[{}] Pixel data is empty", class_name);
		return default_fill_color;
	}
	return colors_ptr()[y * m_width + x];
}

void image::replace(const color src_color, const color dst_color) noexcept {
	if (m_data.empty()) return;

	const auto length{ m_width * m_height };
	auto data{ colors_ptr() };
	for (auto index{ 0u }; index < length; ++index) {
		if (auto &&color{ data[index] }; color == src_color) {
			color = dst_color;
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
	for (auto index{ 0u }; index < num_of_pixels; index += color_count) {
		std::memcpy(&m_data[index], raw_color, color_count);
	}
}

void image::flip(const flip_direction dir) noexcept {
	if (m_data.empty()) {
		return;
	}

	if (flip_direction::both == dir) {
		auto colors{ colors_ptr() };
		const auto length{ m_width * m_height };
		for (auto i{ 0u }, j{ length - 1 }; i < length / 2; ++i, --j) {
			std::swap(colors[i], colors[j]);
		}
	} else if (flip_direction::vertical == dir) {
		for (auto x{ 0u }; x < m_width; ++x) {
			for (auto y{ 0u }; y < m_height / 2; ++y) {
				std::swap(pixel(x, y), pixel(x, m_height - y - 1));
			}
		}
	} else if (flip_direction::horizontal == dir) {
		for (auto y{ 0u }; y < m_height; ++y) {
			for (auto x{ 0u }; x < m_width / 2; ++x) {
				std::swap(pixel(x, y), pixel(m_width - x - 1, y));
			}
		}
	}
}

// void image::copy(const point2<u32> &pos, const image &other, const rect<u32> &source_rect, copy_policy policy) {
// 	if (empty() || other.empty()) {
// 		constexpr static auto b2str{ [](auto value) { return value ? "empty" : "not empty"; } };
// 		spdlog::warn("[{}] Cannot copy empty images. (this: {}, other: {})",
// 			class_name, b2str(empty()), b2str(other.empty()));
// 		return;
// 	}

// 	if (pos.at(0) > m_width || pos.at(1) > m_height) {
// 		utils::error::out_of_range("[golxzn::core::types::image] copy: out of range!");
// 	}

// 	const rect<u32> frame{ reduce_rect(pos, other, source_rect, policy) };



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

	if (x == 0 && y == 0 && w == m_width) {
		if (h == m_height) return; // Nothing to do.
		m_height = h;
		m_data.resize(bytes_count());
		return;
	}

	const auto start_offset{ y * m_width * color_count };
	const auto line_start_offset{ x * color_count };
	const auto line_end_offset{ (m_width - (x + w)) * color_count };

	const auto new_line_bytes_length{ w * color_count };
	const auto new_bytes_length{ new_line_bytes_length * h };
	bytes data(new_bytes_length);
	auto extract_pos{ start_offset };

	for (u32 column{}; column < h; ++column) {
		const auto column_offset{ column * new_line_bytes_length };
		extract_pos += line_start_offset;
		for (u32 row{}; row < new_line_bytes_length; row += color_count, extract_pos += color_count) {
			std::copy_n(&m_data[extract_pos], color_count, &data[column_offset + row]);
		}
		extract_pos += line_end_offset;
	}

	m_data = std::move(data);
	m_width = w;
	m_height = h;
}

void image::crop(const rect<u32> &crop_rect) noexcept {
	crop(crop_rect.x, crop_rect.y, crop_rect.width, crop_rect.height);
}

// void image::expand(const rect<u32> &rect, const color fill_color) noexcept {

// }

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

// rect<u32> image::reduce_rect(const point2<u32> &pos, const image &img,
// 	const rect<u32> &source_rect, copy_policy policy) const noexcept {

// 	rect<u32> frame{ source_rect };
// 	if (frame.right() > img.m_width)   frame.width = img.m_width - frame.x;
// 	if (frame.bottom() > img.m_height) frame.height = img.m_height - frame.y;

// 	if (copy_policy::discard == policy) {
// 		if (frame.right() > m_width)   frame.width = m_width - frame.x;
// 		if (frame.bottom() > m_height) frame.height = m_height - frame.y;
// 	}
// 	return frame;
// }

const color *const image::colors_ptr() const noexcept {
	return reinterpret_cast<const color *>(m_data.data());
}
color *image::colors_ptr() noexcept {
	return reinterpret_cast<color *>(m_data.data());
}
} // namespace golxzn::core::types
