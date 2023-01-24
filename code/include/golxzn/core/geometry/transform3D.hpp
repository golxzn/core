#pragma once

#include <optional>
#include <golxzn/core/math/matrix.hpp>
#include <golxzn/core/math/vector.hpp>
#include <golxzn/core/math/angle.hpp>

namespace golxzn::core::geometry {

class Transform3D {
public:
	static constexpr vec3f32 DefaultPivot{};

	constexpr Transform3D() = default;

	constexpr explicit Transform3D(mat4f32 &&value) noexcept : mMatrix{ std::move(value) } {}
	constexpr explicit Transform3D(mat4f32 &&value, vec3f32 &&pivot) noexcept
		: mMatrix{ std::move(value) }, mPivot{ std::move(pivot) } {}

	constexpr explicit Transform3D(const mat4f32 &value, const vec3f32 &pivot = DefaultPivot) noexcept
		: mMatrix{ value }, mPivot{ pivot } {}

	constexpr Transform3D(const Transform3D &) = default;
	constexpr Transform3D(Transform3D &&) noexcept = default;
	Transform3D &operator=(const Transform3D &) = default;
	Transform3D &operator=(Transform3D &&) noexcept = default;

	[[nodiscard]] vec3f32 size() const noexcept;
	[[nodiscard]] f32 volume() const noexcept;
	[[nodiscard]] vec3f32 position() const noexcept;
	[[nodiscard]] const vec3f32 &pivot() const noexcept;

	const mat4f32 &getMatrix() const noexcept;
	mat4f32 &&matrix() noexcept;

	Transform3D &setPivot(const vec3f32 &pivot);

	Transform3D &translate(const f32 dx, const f32 dy, const f32 dz);
	Transform3D &translate(const vec3f32 &offset);

	Transform3D &scale(const f32 xScale, const f32 yScale, const f32 zScale);
	Transform3D &scale(const vec3f32 &scale);
	Transform3D &scale(const f32 scale);

	Transform3D &shear(const math::anglef32 xsh, const math::anglef32 ysh, const math::anglef32 zsh);

	Transform3D &rotate(const math::anglef32 degrees, const vec3f32 &axis);
	Transform3D &reset();

private:
	mat4f32 mMatrix{};
	vec3f32 mPivot{ DefaultPivot };
};

} // namespace golxzn::core::geometry
