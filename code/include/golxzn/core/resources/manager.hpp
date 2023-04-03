#pragma once

#include <vector>
#include <string>
#include <string_view>

#include "golxzn/core/export.hpp"
#include "golxzn/core/types.hpp"
#include "golxzn/core/types/image.hpp"

namespace golxzn::core::resources {

class GOLXZN_EXPORT manager {
	static constexpr std::string_view class_name{ "resources::manager" };
	static constexpr std::string_view url_separator{ "//" };

	using LoadHandler = std::function<bytes(const std::string_view)>;
	using SaveHandler = std::function<bool(const std::string_view, const u8 *, const u32)>;
	using ExistHandler = std::function<bool(const std::string_view)>;
public:
	static constexpr std::string_view default_asset_directory{ "assets" };
	static constexpr std::string_view default_app_name{ "core_unnamed" };

	static constexpr std::string_view resources_url{ "res://" };
	static constexpr std::string_view user_url{ "user://" };
	static constexpr std::string_view http_url{ "http://" };

	enum class write_mode {
		rewrite,
		append
	};
	enum class image_type { unknown, png, bmp, tga, /*hdr,*/ jpg };

	static void initialize(const std::string_view application_name = default_app_name,
		const std::string_view assets_directory_name = default_asset_directory);

	static void set_write_mode(const write_mode mode) noexcept;
	static void reset_write_mode() noexcept;

	[[nodiscard]] static bytes load_binary(const std::string_view path);
	[[nodiscard]] static std::string load_string(const std::string_view path);
	[[nodiscard]] static types::image::ref load_image(const std::string_view path);

	static bool save_binary(const std::string_view path, const bytes &data);
	static bool save_string(const std::string_view path, const std::string_view data);
	static bool save_image(const std::string_view path, const types::image::ref &data);

	[[nodiscard]] static bool exists(const std::string_view path) noexcept;

	[[nodiscard]] static bool is_http(const std::string_view path) noexcept;
	[[nodiscard]] static bool is_resource(const std::string_view path) noexcept;
	[[nodiscard]] static bool is_user(const std::string_view path) noexcept;

	[[nodiscard]] static const std::error_code &last_error() noexcept;
	[[nodiscard]] static const fs::path &assets_directory() noexcept;
	[[nodiscard]] static const fs::path &user_directory() noexcept;

	[[nodiscard]] static image_type type_by_extension(const std::string_view extension) noexcept;
	[[nodiscard]] static std::string extension_by_type(const image_type type) noexcept;

private:
	static bool initialized;
	static write_mode writing_mode;
	static fs::path assets_root;
	static fs::path user_root;
	static std::error_code err;
	static const umap<std::string_view, LoadHandler> load_handlers;
	static const umap<std::string_view, SaveHandler> save_handlers;
	static const umap<std::string_view, ExistHandler> exist_handlers;

	static bytes load_from(const fs::path &path);
	static bytes load_from_http(const fs::path &path);

	static bool save_to(const fs::path &path, const u8 *data, const u32 size);
	static bool save_to_http(const fs::path &path, const u8 *data, const u32 size);

	static bool exists_in(const fs::path &path) noexcept;
	static bool exists_in_http(const fs::path &path) noexcept;

	static fs::path build_path(const fs::path &prefix, const std::string_view path,
		const std::string_view prefix_to_replace);

	static void setup_assets_root(const std::string_view assets_name);
	static void setup_user_root(const std::string_view app_name);
};

} // namespace golxzn::core::resources

namespace golxzn::core {
using res_man = resources::manager;
} // namespace golxzn::core
