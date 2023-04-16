#pragma once

#include "golxzn/core/aliases.hpp"
#include "golxzn/core/utils/traits.hpp"

namespace golxzn::core::utils {

template<class InstanceType>
struct reference : public std::enable_shared_from_this<std::remove_all_extents_t<InstanceType>> {
	using base_class = std::enable_shared_from_this<std::remove_all_extents_t<InstanceType>>;
	using type = std::remove_all_extents_t<InstanceType>;
	using cref = sptr<const type>;
	using ref = sptr<type>;

	template<class ...Args>
	[[nodiscard]] static ref make(Args ...args) noexcept(noexcept(new type{ std::forward<Args>(args)... })) {
		return ref{ new type{ std::forward<Args>(args)... } };
	}

	[[nodiscard]] cref self() const {
		return base_class::shared_from_this();
	}
	[[nodiscard]] ref self() {
		return base_class::shared_from_this();
	}
};

} // namespace golxzn::core::utils
