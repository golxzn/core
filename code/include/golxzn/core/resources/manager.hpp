#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <golxzn/core/types.hpp>

namespace golxzn::core::resources {

class manager {
	static constexpr std::string_view class_name{ "resources::manager" };
	static constexpr std::string_view url_separator{ "//" };

	using LoadHandler = std::function<bytes(const std::string_view)>;
	using SaveHandler = std::function<bool(const std::string_view, const u8 *, const u32)>;
public:
	static constexpr std::string_view DefaultAssetsDirectory{ "assets" };
	static constexpr std::string_view DefaultAppName{ "core_unnamed" };

	static constexpr std::string_view ResourcesURL{ "res://" };
	static constexpr std::string_view UserURL{ "user://" };
	static constexpr std::string_view HttpURL{ "http://" };

	enum class WriteMode {
		Rewrite,
		Append
	};

	static void initialize(const std::string_view application_name = DefaultAppName,
		const std::string_view assets_directory_name = DefaultAssetsDirectory);

	static void set_write_mode(const WriteMode mode) noexcept;
	static void reset_write_mode() noexcept;

	[[nodiscard]] static bytes load_binary(const std::string_view path);
	[[nodiscard]] static std::string load_string(const std::string_view path);

	static bool save_binary(const std::string_view path, const bytes &data);
	static bool save_string(const std::string_view path, const std::string_view data);

	[[nodiscard]] static const std::error_code &last_error() noexcept;
	[[nodiscard]] static const fs::path &assets_directory() noexcept;
	[[nodiscard]] static const fs::path &user_directory() noexcept;

private:
	static WriteMode write_mode;
	static fs::path assets_root;
	static fs::path user_root;
	static std::error_code err;
	static const umap<std::string_view, LoadHandler> load_handlers;
	static const umap<std::string_view, SaveHandler> save_handlers;

	static bytes load_from(const fs::path &path);
	static bytes load_from_http(const fs::path &path);

	static bool save_to(const fs::path &path, const u8 *data, const u32 size);
	static bool save_to_http(const fs::path &path, const u8 *data, const u32 size);

	static fs::path build_path(const fs::path &prefix, const std::string_view path,
		const std::string_view prefix_to_replace);

	static void setup_assets_root(const std::string_view assets_name);
	static void setup_user_root(const std::string_view app_name);
};

} // namespace golxzn::core::resources

namespace golxzn::core {
using res_man = resources::manager;
} // namespace golxzn::core
