#pragma once

#include <chrono>
#include "golxzn/core/export.hpp"
#include "golxzn/core/aliases.hpp"

namespace golxzn::core {

class time;

class GOLXZN_EXPORT clock {
public:
	using clock_type = std::chrono::steady_clock;

	time elapsed() const noexcept;
	time restart() noexcept;

private:
	clock_type::time_point start_time{ clock_type::now() };
};

} // namespace golxzn::core
