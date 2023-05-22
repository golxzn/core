#include <numeric>
#include <stb_image.h>
#include <stb_image_write.h>
#include <platform_folders.h>
#include <spdlog/spdlog.h>
#include "golxzn/core/resources/manager.hpp"

namespace golxzn::core::resources {

namespace {

using iters_pair = std::pair<u8*, u8*>;
iters_pair make_iters_pair(bytes &data) {
	return std::make_pair(data.data(), data.data() + data.size());
}

int read_callback(void *user, char *data, int size) {
	auto &[begin, end]{ *static_cast<iters_pair*>(user) };
	if (std::distance(begin, end) < size)
		size = static_cast<int>(std::distance(begin, end));

	std::copy_n(begin, size, data);
	begin += size;
	return size;
}
void skip_callback(void *user, int n) {
	auto &[begin, end]{ *static_cast<iters_pair*>(user) };
	begin += n;
}
int eof_callback(void *user) {
	auto &[begin, end]{ *static_cast<iters_pair*>(user) };
	return std::distance(begin, end);
}

} // anonymous namespace

bool manager::initialized{ false };
std::error_code manager::err{};
manager::write_mode manager::writing_mode{ manager::write_mode::rewrite };
fs::path manager::assets_root{ fs::current_path(err) };
fs::path manager::user_root{ fs::path{ sago::getDataHome() } };

const umap<std::string_view, manager::LoadHandler> manager::load_handlers{
	{ manager::resources_url, [](const std::string_view path) { return manager::load_from(manager::build_path(manager::assets_root, path, manager::resources_url)); } },
	{ manager::user_url,      [](const std::string_view path) { return manager::load_from(manager::build_path(manager::user_root, path, manager::user_url)); } },
	{ manager::http_url,      [](const std::string_view path) { return manager::load_from_http(path); } },
};

const umap<std::string_view, manager::LoadLinesHandler> manager::load_lines_handlers{
	{ manager::resources_url, [](const std::string_view path) { return manager::load_lines_from(manager::build_path(manager::assets_root, path, manager::resources_url)); } },
	{ manager::user_url,      [](const std::string_view path) { return manager::load_lines_from(manager::build_path(manager::user_root, path, manager::user_url)); } },
	{ manager::http_url,      [](const std::string_view path) { return manager::load_lines_from_http(path); } },
};

const umap<std::string_view, manager::SaveHandler> manager::save_handlers{
	{ manager::resources_url, [](auto path, auto data, const auto size) { return manager::save_to(manager::build_path(manager::assets_root, path, manager::resources_url), data, size); } },
	{ manager::user_url,      [](auto path, auto data, const auto size) { return manager::save_to(manager::build_path(manager::user_root, path, manager::user_url), data, size); } },
	{ manager::http_url,      [](auto path, auto data, const auto size) { return manager::save_to_http(path, data, size); } },
};

const umap<std::string_view, manager::ExistHandler> manager::exist_handlers{
	{ manager::resources_url, [](const std::string_view path) { return manager::exists_in(manager::build_path(manager::assets_root, path, manager::resources_url)); } },
	{ manager::user_url,      [](const std::string_view path) { return manager::exists_in(manager::build_path(manager::user_root, path, manager::user_url)); } },
	{ manager::http_url,      [](const std::string_view path) { return manager::exists_in_http(path); } },
};

const umap<std::string_view, manager::LastWriteTimeHandler> manager::last_write_time_handlers{
	{ manager::resources_url, [](const std::string_view path) { return manager::last_write_time_for(manager::build_path(manager::assets_root, path, manager::resources_url)); } },
	{ manager::user_url,      [](const std::string_view path) { return manager::last_write_time_for(manager::build_path(manager::user_root, path, manager::user_url)); } },
};


void manager::initialize(const std::string_view application_name, const std::string_view assets_directory_name) {
	if (initialized) return;
	spdlog::info("[{}] Initializing with {} and {}",
		class_name, application_name, assets_directory_name);
	setup_assets_root(assets_directory_name);
	setup_user_root(application_name);
	initialized = true;
}

void manager::set_write_mode(const write_mode mode) noexcept { writing_mode = mode; }
void manager::reset_write_mode() noexcept { writing_mode = write_mode::rewrite; }

bytes manager::load_binary(const std::string_view path) {
	if (path.empty()) {
		spdlog::warn("[{}]: Cannot load binary from empty path.", class_name);
		return {};
	}

	if (const auto url_pos{ path.find(url_separator) }; url_pos != path.npos) {
		const auto url{ path.substr(0, url_pos + url_separator.size()) };
		if (const auto &found{ load_handlers.find(url) }; found != std::end(load_handlers)) {
			return found->second(path);
		} else {
			spdlog::error("[{}] Unknown URL: '{}' in path '{}'", class_name, url, path);
		}
		return {};
	}
	spdlog::error("[{}] Cannot find URL in path '{}'", class_name, path);
	return {};
}

std::string manager::load_string(const std::string_view path) {
	if (const auto data{ load_binary(path) }; !data.empty())
		return std::string{ std::begin(data), std::end(data) };
	return {};
}

std::vector<std::string> manager::load_lines(const std::string_view path) {
	if (path.empty()) {
		spdlog::warn("[{}]: Cannot load lines from empty path.", class_name);
		return {};
	}

	if (const auto url_pos{ path.find(url_separator) }; url_pos != path.npos) {
		const auto url{ path.substr(0, url_pos + url_separator.size()) };
		if (const auto &found{ load_lines_handlers.find(url) }; found != std::end(load_lines_handlers)) {
			return found->second(path);
		} else {
			spdlog::error("[{}] Unknown URL: '{}' in path '{}'", class_name, url, path);
		}
		return {};

	}

	spdlog::error("[{}] Cannot find URL in path '{}'", class_name, path);
	return {};
}

types::image::ref manager::load_image(const std::string_view path) {
	if (path.empty()) {
		spdlog::warn("[{}]: Cannot load image from empty path.", class_name);
		return nullptr;
	}

	if (auto data{ load_binary(path) }; !data.empty()) {
		const auto data_length{ data.size() };
		i32 width{}, height{}, channels{};
		stbi_io_callbacks callbacks{ &read_callback, &skip_callback, &eof_callback };
		auto iters_pair{ make_iters_pair(data) };
		const auto pixels{ stbi_load_from_callbacks(&callbacks, &iters_pair, &width, &height, &channels, 0) };
		if (pixels == nullptr) {
			spdlog::error("[{}] Cannot load image from path '{}': {}",
				class_name, path, stbi_failure_reason());
			return nullptr;
		}

		const auto pixels_count{ width * height };
		auto img{ std::make_shared<types::image>(static_cast<u32>(width), static_cast<u32>(height),
			types::image::to_channel(channels),
			pixels, pixels + (pixels_count * channels)
		) };
		stbi_image_free(pixels);
		return img;
	}
	spdlog::info("[{}] Cannot load image: empty data.", class_name);
	return nullptr;
}

bool manager::save_binary(const std::string_view path, const bytes &data) {
	if (path.empty() || data.empty()) {
		spdlog::warn("[{}]: Cannot save binary: {}", class_name,
			path.empty() ? "empty path" : "empty data");
		return false;
	}

	if (const auto url_pos{ path.find(url_separator) }; url_pos != path.npos) {
		const auto url{ path.substr(0, url_pos + url_separator.size()) };
		if (const auto &found{ save_handlers.find(url) }; found != std::end(save_handlers)) {
			return found->second(path, data.data(), data.size());
		} else {
			spdlog::error("[{}] Unknown URL: '{}' in path '{}'", class_name, url, path);
		}
		return false;
	}
	spdlog::error("[{}] Cannot find URL in path '{}'", class_name, path);
	return false;
}
bool manager::save_string(const std::string_view path, const std::string_view data) {
	if (path.empty() || data.empty()) {
		spdlog::warn("[{}]: Cannot save string: {}", class_name,
			path.empty() ? "empty path" : "empty data");
		return false;
	}


	if (const auto url_pos{ path.find(url_separator) }; url_pos != path.npos) {
		const auto url{ path.substr(0, url_pos + url_separator.size()) };
		if (const auto &found{ save_handlers.find(url) }; found != std::end(save_handlers)) {
			return found->second(path, reinterpret_cast<const u8 *>(data.data()), data.size());
		} else {
			spdlog::error("[{}] Unknown URL: '{}' in path '{}'", class_name, url, path);
		}
		return false;
	}
	spdlog::error("[{}] Cannot find URL in path '{}'", class_name, path);
	return false;
}

bool manager::save_lines(const std::string_view path, const std::vector<std::string> &lines) {
	if (path.empty() || lines.empty()) {
		spdlog::warn("[{}]: Cannot save lines: {}", class_name,
			path.empty() ? "empty path" : "empty data");
		return false;
	}

	const auto size{ std::accumulate(std::begin(lines), std::end(lines), 0,
		[](auto sum, const auto &line) { return sum + line.size() + sizeof(eol); })
	};

	bytes data{};
	data.reserve(size);
	for (const auto &line : lines) {
		std::copy(std::begin(line), std::end(line), std::back_inserter(data));
		data.emplace_back(static_cast<byte>(eol));
	}

	return save_binary(path, data);
}

bool manager::save_image(const std::string_view path, const types::image::ref &img) {
	if (path.empty() || img == nullptr) {
		spdlog::warn("[{}]: Cannot save image: {}",
			class_name, path.empty() ? "empty path" : "empty image");
		return false;
	}
	if (img->empty()) {
		spdlog::warn("[{}]: Cannot save empty image to the '{}' directory", class_name, path);
		return false;
	}

	struct context{
		bytes buffer;
		bool save_status;
	};
	context ctx{ bytes{}, false };
	ctx.buffer.reserve(img->bytes_count() * 2);
	const auto write_func{ [](void *ctx_ptr, void *data, int size) {
		const auto &bytes_ptr{ static_cast<const u8 *>(data) };
		auto ctx{ static_cast<context *>(ctx_ptr) };
		ctx->buffer.insert(std::end(ctx->buffer), bytes_ptr, bytes_ptr + size);
		// ctx->save_status = manager::save_binary(ctx->path, bytes{ bytes_ptr, bytes_ptr + size });
	} };

	const auto img_type{ type_by_extension(path) };
	i32 convert_status{};
	switch(img_type) {
		case image_type::png:
			convert_status = stbi_write_png_to_func(write_func, &ctx,
				img->width(), img->height(), img->color_count(), img->raw().data(), img->stride());
			break;
		case image_type::bmp:
			convert_status = stbi_write_bmp_to_func(write_func, &ctx,
				img->width(), img->height(), img->color_count(), img->raw().data());
			break;
		case image_type::tga:
			convert_status = stbi_write_tga_to_func(write_func, &ctx,
				img->width(), img->height(), img->color_count(), img->raw().data());
			break;
		/*
		case image_type::hdr:
			convert_status = stbi_write_hdr_to_func(write_func, &ctx,
				img->width(), img->height(), img->color_count(), img->data().data());
			break;
		*/
		case image_type::jpg:
			convert_status = stbi_write_jpg_to_func(write_func, &ctx,
				img->width(), img->height(), img->color_count(), img->raw().data(), 90);
			break;
		default:
			spdlog::error("[{}] Cannot save image: format is not supported", class_name);
			return false;
	}
	if (convert_status == 0) {
		spdlog::warn("[{}] Cannot save image to the '{}' directory", class_name, path);
		return false;
	}
	return manager::save_binary(path, ctx.buffer);
}

bool manager::exists(const std::string_view path) noexcept {
	if (path.empty()) return false;

	if (const auto url_pos{ path.find(url_separator) }; url_pos != path.npos) {
		const auto url{ path.substr(0, url_pos + url_separator.size()) };
		if (const auto &found{ exist_handlers.find(url) }; found != std::end(exist_handlers)) {
			return found->second(path);
		} else {
			spdlog::error("[{}] Unknown URL: '{}' in path '{}'", class_name, url, path);
		}
		return false;
	}
	return false;
}

bool manager::is_http(const std::string_view path) noexcept {
	if (path.empty()) return false;

	if (const auto url_pos{ path.find(url_separator) }; url_pos != path.npos) {
		const auto url{ path.substr(0, url_pos + url_separator.size()) };
		return url == http_url;
	}
	return false;
}

bool manager::is_resource(const std::string_view path) noexcept {
	if (path.empty()) return false;

	if (const auto url_pos{ path.find(url_separator) }; url_pos != path.npos) {
		const auto url{ path.substr(0, url_pos + url_separator.size()) };
		return url == resources_url;
	}
	return false;
}

bool manager::is_user(const std::string_view path) noexcept {
	if (path.empty()) return false;

	if (const auto url_pos{ path.find(url_separator) }; url_pos != path.npos) {
		const auto url{ path.substr(0, url_pos + url_separator.size()) };
		return url == user_url;
	}
	return false;
}

std::optional<core::fs::file_time_type> manager::last_write_time(const std::string_view path) noexcept {
	if (path.empty()) return std::nullopt;

	if (const auto url_pos{ path.find(url_separator) }; url_pos != path.npos) {
		const auto url{ path.substr(0, url_pos + url_separator.size()) };
		if (const auto &found{ last_write_time_handlers.find(url) }; found != std::end(last_write_time_handlers)) {
			return found->second(path);
		} else {
			spdlog::error("[{}] Unknown URL: '{}' in path '{}'", class_name, url, path);
		}
	}
	return std::nullopt;
}

const std::error_code &manager::last_error() noexcept {
	return err;
}

const fs::path &manager::assets_directory() noexcept {
	return assets_root;
}
const fs::path &manager::user_directory() noexcept {
	return user_root;
}

manager::image_type manager::type_by_extension(const std::string_view extension) noexcept {
	if (extension.empty()) return image_type::unknown;
	static const core::umap<std::string_view, image_type> extension_types{
		{ ".png",         image_type::png },
		{ ".bmp",         image_type::bmp },
		{ ".tga",         image_type::tga },
		// { ".hdr",         image_type::hdr },
		{ ".jpg",         image_type::jpg },
		{ ".jpeg",        image_type::jpg },
	};

	std::string_view ext{ extension };
	ext.remove_prefix(ext.find_last_of('.'));
	if (const auto &found{ extension_types.find(ext) }; found != std::end(extension_types)) {
		return found->second;
	}
	return image_type::unknown;
}
std::string manager::extension_by_type(const image_type type) noexcept {
	switch (type) {
		case image_type::png: return ".png";
		case image_type::bmp: return ".bmp";
		case image_type::tga: return ".tga";
		// case image_type::hdr: return ".hdr";
		case image_type::jpg: return ".jpg";
		default: break;
	}
	return "";
}

bytes manager::load_from(const fs::path &path) {
	if (!fs::exists(path, err) || !fs::is_regular_file(path, err))
		return {};

	if (fs::bifstream file{ path, std::ios::binary | std::ios::ate }; file.is_open()) {
		const auto size{ static_cast<u32>(file.tellg()) };
		file.seekg(std::ios::beg);
		bytes content(size, u8{});
		file.read(&content[0], size);
		return content;
	}
	return {};
}
bytes manager::load_from_http(const fs::path &path) {
	spdlog::error("[{}] load_from_http isn't implemented yet 😢", class_name);
	return {};
}

std::vector<std::string> manager::load_lines_from(const fs::path &path) {
	if (!fs::exists(path, err) || !fs::is_regular_file(path, err))
		return {};

	/// @todo: Check the speed of this algorithm
	/// I'm not really sure that it's faster than just reading file line by line
	/// and allocate vector a few times
	if (fs::ifstream file{ path, std::ios::ate }; file.is_open()) {
		const auto count{ std::count(std::istreambuf_iterator<char>(file), {}, '\n') };
		file.seekg(std::ios::beg);
		std::vector<std::string> lines;
		lines.reserve(count);
		for (std::string line; std::getline(file, line);)
			lines.emplace_back(std::move(line));
		return lines;
	}
	return {};
}
std::vector<std::string> manager::load_lines_from_http(const fs::path &path) {
	spdlog::error("[{}] load_lines_from_http isn't implemented yet 😢", class_name);
	return {};
}

std::optional<fs::file_time_type> manager::last_write_time_for(const fs::path &path) noexcept {
	const auto result{ fs::last_write_time(path, err) };
	if (err.value() == 0)
		return result;

	spdlog::error("[{}] Cannot get lastwrite_time_for fail: {}", class_name, err.message());
	return std::nullopt;
}

bool manager::save_to(const fs::path &path, const u8 *data, const u32 size) {
	if (!path.has_filename() || size == 0 || data == nullptr)
		return false;

	if (const auto parent_path{ path.parent_path() }; !fs::exists(parent_path, err)) {
		fs::create_directories(parent_path, err);
	}

	const std::ios::openmode mode{
		std::ios::binary | (writing_mode == write_mode::append ? std::ios::app : std::ios::trunc)
	};
	if (fs::bofstream file{ path, mode }; file.is_open()) {
		file.write(data, size);
		return true;
	}

	return false;
}
bool manager::save_to_http(const fs::path &path, const u8 *data, const u32 size) {
	if (!path.has_filename() || size == 0 || data == nullptr)
		return false;
	/// @todo: Implement loading file from the web
	spdlog::error("[{}] save_to_http isn't implemented yet", class_name);
	return false;
}

bool manager::exists_in(const fs::path &path) noexcept {
	if (path.empty()) return false;
	return fs::exists(path, err);
}

bool manager::exists_in_http(const fs::path &path) noexcept {
	if (path.empty()) return false;

	/// @todo: Implement loading file from the web
	spdlog::error("[{}] exist_to_http isn't implemented yet", class_name);
	return false;
}

fs::path manager::build_path(const fs::path &prefix, const std::string_view path,
	const std::string_view prefix_to_replace) {
	return prefix / fs::path{ path.substr(prefix_to_replace.size()) }.make_preferred();
}

void manager::setup_assets_root(const std::string_view assets_name) {
	const fs::path targetDir{ assets_name.empty() ? default_asset_directory : assets_name };

	while (!assets_root.empty()) {
		fs::directory_iterator iter{ assets_root, fs::directory_options::skip_permission_denied, err };
		const auto end{ fs::end(iter) };
		for (; iter != end; iter = iter.increment(err)) {
			const auto &directory{ *iter };
			if (!fs::is_directory(directory, err))
				continue;
			const auto current{ directory / targetDir };
			if (fs::exists(current, err)) {
				assets_root = current;
				spdlog::info("[{}] Assets directory was found in '{}'",
					class_name, assets_root.string());
				return;
			}
		}
		assets_root = assets_root.parent_path();
	}
	assets_root = fs::current_path(err) / targetDir;
	spdlog::warn("[{}] Cannot find '{}' directory. Creating in '{}'",
		class_name, targetDir.string(), assets_root.string());
	fs::create_directories(assets_root, err);
}

void manager::setup_user_root(const std::string_view app_name) {
	user_root /= (app_name.empty() ? default_app_name : app_name);

	if (!fs::exists(user_root, err)) {
		fs::create_directories(user_root, err);
	}

	spdlog::info("[{}] User directory was found in '{}'", class_name, user_root.string());
}

} // namespace golxzn::core::resources
