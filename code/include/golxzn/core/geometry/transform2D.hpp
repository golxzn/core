#pragma once

#include <golxzn/core/math/matrix.hpp>
#include <golxzn/core/math/vector.hpp>
#include <golxzn/core/math/angle.hpp>

namespace golxzn::core::geometry {

class Transform2D {
public:
	static constexpr vec2f32 DefaultPivot{};

	constexpr Transform2D() = default;
	constexpr explicit Transform2D(mat3f32 &&value) noexcept : mMatrix{ std::move(value) } {}
	constexpr explicit Transform2D(mat3f32 &&value, vec2f32 &&pivot) noexcept
		: mMatrix{ std::move(value) }, mPivot{ std::move(pivot) } {}

	constexpr explicit Transform2D(const mat3f32 &value) noexcept : mMatrix{ value } {}
	constexpr explicit Transform2D(const mat3f32 &value, const vec2f32 &pivot) noexcept
		: mMatrix{ value }, mPivot{ pivot } {}

	constexpr Transform2D(const Transform2D &) = default;
	constexpr Transform2D(Transform2D &&) noexcept = default;
	Transform2D &operator=(const Transform2D &) = default;
	Transform2D &operator=(Transform2D &&) noexcept = default;

	[[nodiscard]] vec2f32 size() const noexcept;
	[[nodiscard]] f32 area() const noexcept;
	[[nodiscard]] vec2f32 position() const noexcept;
	[[nodiscard]] const vec2f32 &pivot() const noexcept;

	const mat3f32 &getMatrix() const noexcept;
	mat3f32 &&matrix() noexcept;

	Transform2D &setPivot(const vec2f32 &pivot);

	Transform2D &translate(const f32 dx, const f32 dy);
	Transform2D &translate(const vec2f32 &offset);

	Transform2D &scale(const f32 xScale, const f32 yScale);
	Transform2D &scale(const vec2f32 &scale);
	Transform2D &scale(const f32 scale);

	Transform2D &shear(const math::anglef32 phi, const math::anglef32 psi);

	Transform2D &rotate(const math::anglef32 degrees);
	Transform2D &reset();

private:
	mat3f32 mMatrix{};
	vec2f32 mPivot{ DefaultPivot };
};

using trans3d = Transform2D;

} // namespace golxzn::core::geometry
