#include "golxzn/core/geometry/transform3D.hpp"

namespace golxzn::core::geometry {

vec3f32 Transform3D::size() const noexcept {
	return vec3f32{ mMatrix(0, 0), mMatrix(1, 1), mMatrix(2, 2) };
}
f32 Transform3D::volume() const noexcept {
	return mMatrix(0, 0) * mMatrix(1, 1) * mMatrix(2, 2);
}
vec3f32 Transform3D::position() const noexcept {
	return vec3f32{ mMatrix(0, 3), mMatrix(1, 3), mMatrix(2, 3) };
}
const vec3f32 &Transform3D::pivot() const noexcept {
	return mPivot;
}

const mat4f32 &Transform3D::getMatrix() const noexcept {
	return mMatrix;
}
mat4f32 &&Transform3D::matrix() noexcept {
	return std::move(mMatrix);
}

Transform3D &Transform3D::setPivot(const vec3f32 &pivot) {
	mPivot = pivot;
	return *this;
}

Transform3D &Transform3D::translate(const f32 dx, const f32 dy, const f32 dz) {
	mMatrix(0, 3) += dx;
	mMatrix(1, 3) += dy;
	mMatrix(2, 3) += dz;
	return *this;
}
Transform3D &Transform3D::translate(const vec3f32 &offset) {
	return translate(offset.at(0), offset.at(1), offset.at(2));
}

Transform3D &Transform3D::scale(const f32 xScale, const f32 yScale, const f32 zScale) {
	mMatrix(0, 0) *= xScale;
	mMatrix(1, 1) *= yScale;
	mMatrix(2, 2) *= zScale;
	return *this;
}
Transform3D &Transform3D::scale(const vec3f32 &_scale) {
	return scale(_scale.at(0), _scale.at(1), _scale.at(2));
}
Transform3D &Transform3D::scale(const f32 _scale) {
	return scale(_scale, _scale, _scale);
}

Transform3D &Transform3D::shear(const anglef32 xsh, const anglef32 ysh, const anglef32 zsh) {
	mMatrix(0, 1) = xsh.tan();
	mMatrix(0, 2) = zsh.tan();
	mMatrix(1, 2) = ysh.tan();
	return *this;
}

Transform3D &Transform3D::rotate(const anglef32 degrees, const vec3f32 &axis) {
	translate(-mPivot);
	const auto cos{ degrees.cos() };
	const auto sin{ degrees.sin() };
	if (axis.at(0) != vec3f32::zero) {
		mMatrix(1, 1) *= cos; mMatrix(1, 2) *= -sin;
		mMatrix(2, 1) *= sin; mMatrix(2, 2) *=  cos;
	}
	if (axis.at(1) != vec3f32::zero) {
		mMatrix(0, 0) *= cos; mMatrix(0, 2) *=  sin;
		mMatrix(2, 0) *= -sin; mMatrix(2, 2) *= cos;
	}
	if (axis.at(2) != vec3f32::zero) {
		mMatrix(0, 0) *= cos; mMatrix(0, 1) *= -sin;
		mMatrix(1, 0) *= sin; mMatrix(1, 1) *=  cos;
	}
	return translate(mPivot);
}

Transform3D &Transform3D::reset() {
	mMatrix = mat4f32::identity();
	mPivot = DefaultPivot;
	return *this;
}

Transform3D &Transform3D::operator*=(const Transform3D &rhs) {
	mMatrix *= rhs.mMatrix;
	return *this;
}
Transform3D Transform3D::operator*(const Transform3D &rhs) const {
	return Transform3D{ *this } *= rhs;
}
vec4f32 Transform3D::operator*(const vec4f32 &rhs) const {
	return mMatrix * rhs;
}
vec3f32 Transform3D::operator*(const vec3f32 &rhs) const {
	using namespace types_literals;
	return operator*(vec4f32{ rhs.at(0), rhs.at(1), rhs.at(2), vec4f32::one })
		.as<3>({ 0_usize, 1_usize, 2_usize });
}

Transform3D &Transform3D::apply(vec4f32 &vector) {
	vector = operator*(vector);
	return *this;
}
Transform3D &Transform3D::apply(vec3f32 &vector) {
	vector = operator*(vector);
	return *this;
}
void Transform3D::apply(vec4f32 &vector) const {
	vector = operator*(vector);
}
void Transform3D::apply(vec3f32 &vector) const {
	vector = operator*(vector);
}



} // namespace golxzn::core::geometry
