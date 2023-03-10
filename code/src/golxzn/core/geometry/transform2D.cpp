#include <cmath>

#include "golxzn/core/geometry/transform2D.hpp"

namespace golxzn::core::geometry {

vec2f32 Transform2D::size() const noexcept {
	return vec2f32{ mMatrix(0, 0), mMatrix(1, 1) };
}
f32 Transform2D::area() const noexcept {
	return  mMatrix(0, 0) * mMatrix(1, 1);
}
vec2f32 Transform2D::position() const noexcept {
	return vec2f32{ mMatrix(0, 2), mMatrix(1, 2) };
}
const vec2f32 &Transform2D::pivot() const noexcept {
	return mPivot;
}

const mat3f32 &Transform2D::getMatrix() const noexcept {
	return mMatrix;
}

mat3f32 &&Transform2D::matrix() noexcept {
	return std::move(mMatrix);
}

Transform2D &Transform2D::setPivot(const vec2f32 &pivot) {
	mPivot = pivot;
	return *this;
}

Transform2D &Transform2D::translate(const f32 dx, const f32 dy) {
	mMatrix(0, 2) += dx;
	mMatrix(1, 2) += dy;
	return *this;
}

Transform2D &Transform2D::translate(const vec2f32 &offset) {
	return translate(offset.at(0), offset.at(1));
}

Transform2D &Transform2D::scale(const f32 xScale, const f32 yScale) {
	if (mPivot == DefaultPivot) {
		mMatrix(0, 0) *= xScale;
		mMatrix(1, 1) *= yScale;
		return *this;
	}
	const f32 x{ mMatrix(0, 2) };
	const f32 y{ mMatrix(1, 2) };
	mMatrix(0, 0) *= xScale;
	mMatrix(1, 1) *= yScale;
	return translate(
		(x - x * xScale) * (vec2f32::one - mPivot.at(0)),
		(y - y * yScale) * (vec2f32::one - mPivot.at(1))
	);
}
Transform2D &Transform2D::scale(const vec2f32 &_scale) {
	return scale(_scale.at(0), _scale.at(1));
}
Transform2D &Transform2D::scale(const f32 _scale) {
	return scale(_scale, _scale);
}

Transform2D &Transform2D::shear(const anglef32 phi, const anglef32 psi) {
	mMatrix(0, 1) = phi.tan();
	mMatrix(1, 0) = psi.tan();
	return *this;
}

Transform2D &Transform2D::rotate(const anglef32 degrees) {
	translate(-mPivot);
	const f32 sin{ degrees.sin() };
	const f32 cos{ degrees.cos() };

	const auto a11{ mMatrix(0, 0) }; const auto a12{ mMatrix(0, 1) }; const auto a13{ mMatrix(0, 2) };
	const auto a21{ mMatrix(1, 0) }; const auto a22{ mMatrix(1, 1) }; const auto a23{ mMatrix(1, 2) };
	const auto a31{ mMatrix(2, 0) }; const auto a32{ mMatrix(2, 1) }; const auto a33{ mMatrix(2, 2) };

	mMatrix(0, 0) = a11 * cos + a12 * sin; mMatrix(0, 1) = a12 * cos - a11 * sin;
	mMatrix(1, 0) = a21 * cos + a22 * sin; mMatrix(1, 1) = a22 * cos - a21 * sin;
	mMatrix(2, 0) = a31 * cos + a32 * sin; mMatrix(2, 1) = a32 * cos - a31 * sin;

	return translate(mPivot);
}

Transform2D &Transform2D::rotate_opt(const anglef32 degrees) {
	translate(-mPivot);
	const f32 sin{ degrees.sin() };
	const f32 cos{ degrees.cos() };

	if (std::abs(sin - cos) < std::numeric_limits<f32>::epsilon()) {
		mMatrix(0, 0) = mMatrix(1, 1) = cos * cos + sin * sin;
		mMatrix(0, 1) = cos * sin - sin * cos;
		mMatrix(1, 0) = sin * cos + cos * sin;
		return translate(mPivot);
	}

	const auto a11{ mMatrix(0, 0) }; const auto a12{ mMatrix(0, 1) }; const auto a13{ mMatrix(0, 2) };
	const auto a21{ mMatrix(1, 0) }; const auto a22{ mMatrix(1, 1) }; const auto a23{ mMatrix(1, 2) };
	const auto a31{ mMatrix(2, 0) }; const auto a32{ mMatrix(2, 1) }; const auto a33{ mMatrix(2, 2) };

	mMatrix(0, 0) = a11 * cos + a12 * sin; mMatrix(0, 1) = a12 * cos - a11 * sin;
	mMatrix(1, 0) = a21 * cos + a22 * sin; mMatrix(1, 1) = a22 * cos - a21 * sin;
	mMatrix(2, 0) = a31 * cos + a32 * sin; mMatrix(2, 1) = a32 * cos - a31 * sin;

	return translate(mPivot);
}

Transform2D &Transform2D::reset() {
	mMatrix = mat3f32::identity();
	mPivot = DefaultPivot;
	return *this;
}

Transform2D &Transform2D::operator*=(const Transform2D &rhs) {
	mMatrix *= rhs.mMatrix;
	return *this;
}
Transform2D Transform2D::operator*(const Transform2D &rhs) const {
	return Transform2D{ *this } *= rhs;
}
vec3f32 Transform2D::operator*(const vec3f32 &rhs) const {
	return mMatrix * rhs;
}
vec2f32 Transform2D::operator*(const vec2f32 &rhs) const {
	using namespace types_literals;
	return operator*(vec3f32{ rhs.at(0), rhs.at(1), rhs.one }).as<2>({ 0_usize, 1_usize });
}

Transform2D &Transform2D::apply(vec3f32 &point) {
	point = operator*(point);
	return *this;
}
Transform2D &Transform2D::apply(vec2f32 &point) {
	point = operator*(point);
	return *this;
}

void Transform2D::apply(vec3f32 &point) const {
	point = operator*(point);
}
void Transform2D::apply(vec2f32 &point) const {
	point = operator*(point);
}

} // namespace golxzn::core::geometry
