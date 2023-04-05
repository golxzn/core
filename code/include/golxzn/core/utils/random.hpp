#pragma once

#include <random>
#include "golxzn/core/export.hpp"
#include "golxzn/core/aliases.hpp"

namespace golxzn::core::utils {

struct GOLXZN_EXPORT random final {

	GOLXZN_STATIC_CLASS(random);

	template<class T>
	using default_distr_v = std::conditional_t<
		std::is_floating_point_v<T>,
		std::uniform_real_distribution<T>,
		std::uniform_int_distribution<T>
	>;

	template<class T, class Engine = std::mt19937_64, class Distribution = default_distr_v<T>>
	[[nodiscard]] static T range(const T min, const T max) noexcept {
		static std::random_device device{};
		static Engine engine{ device() };
		Distribution distribution{ min, max };
		return distribution(engine);
	}
};

} // namespace golxzn::core::utils
