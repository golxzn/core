#pragma once

#include <variant>
#include <golxzn/core/types.hpp>
#include <golxzn/core/constants.hpp>

namespace golxzn::core {

template<class ValueType>
class angle {
public:
	using value_type = std::remove_all_extents_t<ValueType>;
	struct radians { value_type value{}; };
	struct degrees { value_type value{}; };

	template<class AngleType, std::enable_if_t<
		std::is_same_v<AngleType, radians> || std::is_same_v<AngleType, degrees>, bool> = false>
	constexpr explicit angle(const AngleType angle) noexcept : mValue{ angle } { }

	constexpr angle() noexcept = default;
	constexpr angle(const angle &) noexcept = default;
	constexpr angle(angle &&) noexcept = default;
	angle &operator=(const angle &) noexcept = default;
	angle &operator=(angle &&) noexcept = default;

	[[nodiscard]] constexpr bool isDegrees() const noexcept {
		return std::holds_alternative<degrees>(mValue);
	}
	[[nodiscard]] constexpr bool isRadians() const noexcept {
		return std::holds_alternative<radians>(mValue);
	}

	template<class AngleType, std::enable_if_t<
		std::is_same_v<AngleType, radians> || std::is_same_v<AngleType, degrees>, bool> = false>
	[[nodiscard]] constexpr value_type as() const noexcept {
		if constexpr (std::is_same_v<AngleType, radians>) {
			if (isRadians()) return std::get<radians>(mValue).value;
			return degrees_to_radians(std::get<degrees>(mValue).value);
		}
		if (isDegrees()) return std::get<degrees>(mValue).value;
		return radians_to_degrees(std::get<radians>(mValue).value);
	}

	[[nodiscard]] constexpr value_type value() const noexcept {
		if (isDegrees()) return std::get<degrees>(mValue).value;
		return std::get<radians>(mValue).value;
	}

	[[nodiscard]] constexpr value_type sin() const noexcept {
		return std::sin(as<radians>());
	}
	[[nodiscard]] constexpr value_type cos() const noexcept {
		return std::cos(as<radians>());
	}
	[[nodiscard]] constexpr value_type tan() const noexcept {
		return std::tan(as<radians>());
	}

	[[nodiscard]] constexpr bool operator==(const angle &other) const noexcept {
		return as<radians>() == other.as<radians>();
	}
	[[nodiscard]] constexpr bool operator!=(const angle &other) const noexcept {
		return as<radians>() != other.as<radians>();
	}

	[[nodiscard]] constexpr angle &operator+=(const angle &other) noexcept {
		if (isDegrees()) {
			std::get<degrees>(mValue).value += other.as<degrees>();
		} else if (isRadians()) {
			std::get<radians>(mValue).value += other.as<radians>();
		} else {
			mValue = other.mValue;
		}

		return *this;
	}
	[[nodiscard]] constexpr angle &operator-=(const angle &other) noexcept {
		if (isDegrees()) {
			std::get<degrees>(mValue).value -= other.as<degrees>();
		} else if (isRadians()) {
			std::get<radians>(mValue).value -= other.as<radians>();
		} else if (other.isDegrees()) {
			mValue = degrees{ -other.as<degrees>() };
		} else if (other.isRadians()) {
			mValue = radians{ -other.as<radians>() };
		}

		return *this;
	}

	static constexpr value_type radians_to_degrees(const value_type radians) noexcept {
		return radians / constants::pi_v<value_type> * static_cast<value_type>(180.0);
	}
	static constexpr value_type degrees_to_radians(const value_type degrees) noexcept {
		return degrees / static_cast<value_type>(180.0) * constants::pi_v<value_type>;
	}
private:
	std::variant<radians, degrees> mValue{ degrees{ static_cast<value_type>(0) } };
};

using anglef16 = angle<f16>;
using anglef32 = angle<f32>;
using anglef64 = angle<f64>;

namespace angle_literals {

[[nodiscard]] constexpr auto operator""_deg(const f64 value) noexcept {
	return core::angle<f32>{ core::angle<f32>::degrees{ static_cast<f32>(value) } };
}
[[nodiscard]] constexpr auto operator""_deg(const u64 value) noexcept {
	return core::angle<f32>{ core::angle<f32>::degrees{ static_cast<f32>(value) } };
}

[[nodiscard]] constexpr auto operator""_degf64(const f64 value) noexcept {
	return core::angle<f64>{ core::angle<f64>::degrees{ static_cast<f64>(value) } };
}
[[nodiscard]] constexpr auto operator""_degf32(const f64 value) noexcept {
	return core::angle<f32>{ core::angle<f32>::degrees{ static_cast<f32>(value) } };
}
[[nodiscard]] constexpr auto operator""_degf16(const f64 value) noexcept {
	return core::angle<f16>{ core::angle<f16>::degrees{ static_cast<f16>(value) } };
}
[[nodiscard]] constexpr auto operator""_degf64(const u64 value) noexcept {
	return core::angle<f64>{ core::angle<f64>::degrees{ static_cast<f64>(value) } };
}
[[nodiscard]] constexpr auto operator""_degf32(const u64 value) noexcept {
	return core::angle<f32>{ core::angle<f32>::degrees{ static_cast<f32>(value) } };
}
[[nodiscard]] constexpr auto operator""_degf16(const u64 value) noexcept {
	return core::angle<f16>{ core::angle<f16>::degrees{ static_cast<f16>(value) } };
}

[[nodiscard]] constexpr auto operator""_rad(const f64 value) noexcept {
	return core::angle<f32>{ core::angle<f32>::radians{ static_cast<f32>(value) } };
}
[[nodiscard]] constexpr auto operator""_rad(const u64 value) noexcept {
	return core::angle<f32>{ core::angle<f32>::radians{ static_cast<f32>(value) } };
}

[[nodiscard]] constexpr auto operator""_radf64(const f64 value) noexcept {
	return core::angle<f64>{ core::angle<f64>::radians{ static_cast<f64>(value) } };
}
[[nodiscard]] constexpr auto operator""_radf32(const f64 value) noexcept {
	return core::angle<f32>{ core::angle<f32>::radians{ static_cast<f32>(value) } };
}
[[nodiscard]] constexpr auto operator""_radf16(const f64 value) noexcept {
	return core::angle<f16>{ core::angle<f16>::radians{ static_cast<f16>(value) } };
}
[[nodiscard]] constexpr auto operator""_radf64(const u64 value) noexcept {
	return core::angle<f64>{ core::angle<f64>::radians{ static_cast<f64>(value) } };
}
[[nodiscard]] constexpr auto operator""_radf32(const u64 value) noexcept {
	return core::angle<f32>{ core::angle<f32>::radians{ static_cast<f32>(value) } };
}
[[nodiscard]] constexpr auto operator""_radf16(const u64 value) noexcept {
	return core::angle<f16>{ core::angle<f16>::radians{ static_cast<f16>(value) } };
}


} // namespace angle_literals

} // namespace golxzn::core

namespace golxzn {
namespace angle_literals = core::angle_literals;
} // namespace golxzn

