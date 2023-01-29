#pragma once

#include "golxzn/core/math/point.hpp"

namespace golxzn::core {

template<class T, usize Count>
using vec = point<T, Count>;

template<class T> using vec2 = point2<T>;

using vec2f32 = point2f32;
using vec2f = point2f;
using vec2f64 = point2f64;
using vec2d = point2d;
using vec2i32 = point2i32;
using vec2i = point2i;

template<class T> using vec3 = point3<T>;

using vec3f32 =  point3f32;
using vec3f =  point3f;
using vec3f64 =  point3f64;
using vec3d =  point3d;
using vec3i32 =  point3i32;
using vec3i =  point3i;

template<class T> using vec4 = point4<T>;

using vec4f32 = point4f32;
using vec4f = point4f;
using vec4f64 = point4f64;
using vec4d = point4d;
using vec4i32 = point4i32;
using vec4i = point4i;

} // namespace golxzn::core
