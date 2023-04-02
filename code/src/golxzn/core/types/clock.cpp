#include <utility>

#include "golxzn/core/types/time.hpp"
#include "golxzn/core/types/clock.hpp"

namespace golxzn::core {

time clock::elapsed() const noexcept {
	return time{
		std::chrono::duration_cast<std::chrono::microseconds>(clock_type::now() - start_time)
	};
}

time clock::restart() noexcept {
	const auto now = clock_type::now();
	return time{
		std::chrono::duration_cast<std::chrono::microseconds>(now - std::exchange(start_time, now))
	};
}

} // namespace golxzn::core
