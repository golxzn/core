#pragma once

#include "golxzn/core/aliases.hpp"
#include "golxzn/core/utils/traits.hpp"

namespace golxzn::core::utils {

template<class InstanceType>
struct instantiated {
	using type = InstanceType;

	template<class... Args, std::enable_if_t<std::is_constructable_v<type, Args>, bool> = false>
	sptr make(Args ... args) const noexcept(noexcept(type{ std::forward<Args>(args)... })) {
		return std::make_shared<type>(std::forward<Args>(args)...);
	}
};

} // namespace golxzn::core::utils
