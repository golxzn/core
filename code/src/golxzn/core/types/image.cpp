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
u32 image::bytes_count() const noexcept {
	return m_width * m_height * color_count;
}
const bytes &image::data() const noexcept {
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

void image::flip(const std::bitset<2> &dir) noexcept {
	if (m_data.empty() || dir.none()) {
		return;
	}

	if (dir.all()) {
		for (auto x{ 0u }; x < m_width / 2; ++x) {
			for (auto y{ 0u }; y < m_height / 2; ++y) {
				std::swap(pixel(x, y), pixel(m_width - x - 1, m_height - y - 1));
			}
		}
	} else if (dir.test(flip_vertical)) {
		for (auto x{ 0u }; x < m_width; ++x) {
			for (auto y{ 0u }; y < m_height / 2; ++y) {
				std::swap(pixel(x, y), pixel(x, m_height - y - 1));
			}
		}
	} else if (dir.test(flip_horizontal)) {
		for (auto y{ 0u }; y < m_height; ++y) {
			for (auto x{ 0u }; x < m_width / 2; ++x) {
				std::swap(pixel(x, y), pixel(m_width - x - 1, y));
			}
		}
	}
}

// void image::copy(const point2<u32> &pos, const image &other, const rect<u32> &source_rect) {
// 	if (pos.at(0) > m_width || pos.at(1) > m_height) {
// 		utils::error::out_of_range("[golxzn::core::types::image] copy: out of range!");
// 	}
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

const color *const image::colors_ptr() const noexcept {
	return reinterpret_cast<const color *>(m_data.data());
}
color *image::colors_ptr() noexcept {
	return reinterpret_cast<color *>(m_data.data());
}

} // namespace golxzn::core::types
