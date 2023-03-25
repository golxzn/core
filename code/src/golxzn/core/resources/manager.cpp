#include <platform_folders.h>
#include <spdlog/spdlog.h>
#include "golxzn/core/resources/manager.hpp"

namespace golxzn::core::resources {

std::error_code manager::err{};
manager::WriteMode manager::write_mode{ manager::WriteMode::Rewrite };
fs::path manager::assets_root{ fs::current_path(err) };
fs::path manager::user_root{ fs::path{ sago::getDataHome() } };

const umap<std::string_view, manager::LoadHandler> manager::load_handlers{
	{ manager::ResourcesURL, [](const std::string_view path) { return manager::load_from(manager::build_path(manager::assets_root, path, manager::ResourcesURL)); } },
	{ manager::UserURL,      [](const std::string_view path) { return manager::load_from(manager::build_path(manager::user_root, path, manager::UserURL)); } },
	{ manager::HttpURL,      [](const std::string_view path) { return manager::load_from_http(path); } },
};

const umap<std::string_view, manager::SaveHandler> manager::save_handlers{
	{ manager::ResourcesURL, [](auto path, auto data, const auto size) { return manager::save_to(manager::build_path(manager::assets_root, path, manager::ResourcesURL), data, size); } },
	{ manager::UserURL,      [](auto path, auto data, const auto size) { return manager::save_to(manager::build_path(manager::user_root, path, manager::UserURL), data, size); } },
	{ manager::HttpURL,      [](auto path, auto data, const auto size) { return manager::save_to_http(path, data, size); } },
};


void manager::initialize(const std::string_view application_name, const std::string_view assets_directory_name) {
	spdlog::info("[{}] Initializing with {} and {}",
		class_name, application_name, assets_directory_name);
	setup_assets_root(assets_directory_name);
	setup_user_root(application_name);
}

void manager::set_write_mode(const WriteMode mode) noexcept { write_mode = mode; }
void manager::reset_write_mode() noexcept { write_mode = WriteMode::Rewrite; }

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

const std::error_code &manager::last_error() noexcept {
	return err;
}

const fs::path &manager::assets_directory() noexcept {
	return assets_root;
}
const fs::path &manager::user_directory() noexcept {
	return user_root;
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
	spdlog::error("[{}] load_from_http isn't implemented yet", class_name);
	return {};
}

bool manager::save_to(const fs::path &path, const u8 *data, const u32 size) {
	if (!path.has_filename() || size == 0 || data == nullptr)
		return false;

	if (const auto parent_path{ path.parent_path() }; !fs::exists(parent_path, err)) {
		fs::create_directories(parent_path, err);
	}

	const std::ios::openmode mode{
		std::ios::binary | (write_mode == WriteMode::Append ? std::ios::app : std::ios::trunc)
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

fs::path manager::build_path(const fs::path &prefix, const std::string_view path,
	const std::string_view prefix_to_replace) {
	return prefix / fs::path{ path.substr(prefix_to_replace.size()) }.make_preferred();
}

void manager::setup_assets_root(const std::string_view assets_name) {
	const fs::path targetDir{ assets_name.empty() ? DefaultAssetsDirectory : assets_name };

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
	user_root /= (app_name.empty() ? DefaultAppName : app_name);

	if (!fs::exists(user_root, err)) {
		fs::create_directories(user_root, err);
	}

	spdlog::info("[{}] User directory was found in '{}'", class_name, user_root.string());
}

} // namespace golxzn::core::resources
