#pragma once

#include <limits>

#include <golxzn/core/types.hpp>

namespace golxzn::core::constants {

constexpr u8 invalid_id_u8{ std::numeric_limits<u8>::max() };
constexpr u16 invalid_id_u16{ std::numeric_limits<u16>::max() };
constexpr u32 invalid_id_u32{ std::numeric_limits<u32>::max() };
constexpr u64 invalid_id_u64{ std::numeric_limits<u64>::max() };

template<class T> struct invalid_value {
	static_assert(std::false_type<T>::value, "The constant value for the type is not defined.");
};

template<class T> inline constexpr T e_v         { invalid_value<T>{} };
template<class T> inline constexpr T log2e_v     { invalid_value<T>{} };
template<class T> inline constexpr T log10e_v    { invalid_value<T>{} };
template<class T> inline constexpr T pi_v        { invalid_value<T>{} };
template<class T> inline constexpr T inv_pi_v    { invalid_value<T>{} };
template<class T> inline constexpr T inv_sqrtpi_v{ invalid_value<T>{} };
template<class T> inline constexpr T ln2_v       { invalid_value<T>{} };
template<class T> inline constexpr T ln10_v      { invalid_value<T>{} };
template<class T> inline constexpr T sqrt2_v     { invalid_value<T>{} };
template<class T> inline constexpr T sqrt3_v     { invalid_value<T>{} };
template<class T> inline constexpr T inv_sqrt3_v { invalid_value<T>{} };
template<class T> inline constexpr T egamma_v    { invalid_value<T>{} };
template<class T> inline constexpr T phi_v       { invalid_value<T>{} };

template<> inline constexpr f64 e_v<f64>         { 2.71828'18284'59045L };
template<> inline constexpr f64 log2e_v<f64>     { 1.44269'50408'88963L };
template<> inline constexpr f64 log10e_v<f64>    { 0.43429'44819'03251L };
template<> inline constexpr f64 pi_v<f64>        { 3.14159'26535'89793L };
template<> inline constexpr f64 inv_pi_v<f64>    { 0.31830'98861'83790L };
template<> inline constexpr f64 inv_sqrtpi_v<f64>{ 0.56418'95835'47756L };
template<> inline constexpr f64 ln2_v<f64>       { 0.69314'71805'59945L };
template<> inline constexpr f64 ln10_v<f64>      { 2.30258'50929'94046L };
template<> inline constexpr f64 sqrt2_v<f64>     { 1.41421'35623'73095L };
template<> inline constexpr f64 sqrt3_v<f64>     { 1.73205'08075'68877L };
template<> inline constexpr f64 inv_sqrt3_v<f64> { 0.57735'02691'89625L };
template<> inline constexpr f64 egamma_v<f64>    { 0.57721'56649'01532L };
template<> inline constexpr f64 phi_v<f64>       { 1.61803'39887'49895L };

template<> inline constexpr f32 e_v<f32>         { static_cast<f32>(e_v<f64>)          };
template<> inline constexpr f32 log2e_v<f32>     { static_cast<f32>(log2e_v<f64>)      };
template<> inline constexpr f32 log10e_v<f32>    { static_cast<f32>(log10e_v<f64>)     };
template<> inline constexpr f32 pi_v<f32>        { static_cast<f32>(pi_v<f64>)         };
template<> inline constexpr f32 inv_pi_v<f32>    { static_cast<f32>(inv_pi_v<f64>)     };
template<> inline constexpr f32 inv_sqrtpi_v<f32>{ static_cast<f32>(inv_sqrtpi_v<f64>) };
template<> inline constexpr f32 ln2_v<f32>       { static_cast<f32>(ln2_v<f64>)        };
template<> inline constexpr f32 ln10_v<f32>      { static_cast<f32>(ln10_v<f64>)       };
template<> inline constexpr f32 sqrt2_v<f32>     { static_cast<f32>(sqrt2_v<f64>)      };
template<> inline constexpr f32 sqrt3_v<f32>     { static_cast<f32>(sqrt3_v<f64>)      };
template<> inline constexpr f32 inv_sqrt3_v<f32> { static_cast<f32>(inv_sqrt3_v<f64>)  };
template<> inline constexpr f32 egamma_v<f32>    { static_cast<f32>(egamma_v<f64>)     };
template<> inline constexpr f32 phi_v<f32>       { static_cast<f32>(phi_v<f64>)        };

template<> inline constexpr f16 e_v<f16>         { static_cast<f16>(e_v<f64>)          };
template<> inline constexpr f16 log2e_v<f16>     { static_cast<f16>(log2e_v<f64>)      };
template<> inline constexpr f16 log10e_v<f16>    { static_cast<f16>(log10e_v<f64>)     };
template<> inline constexpr f16 pi_v<f16>        { static_cast<f16>(pi_v<f64>)         };
template<> inline constexpr f16 inv_pi_v<f16>    { static_cast<f16>(inv_pi_v<f64>)     };
template<> inline constexpr f16 inv_sqrtpi_v<f16>{ static_cast<f16>(inv_sqrtpi_v<f64>) };
template<> inline constexpr f16 ln2_v<f16>       { static_cast<f16>(ln2_v<f64>)        };
template<> inline constexpr f16 ln10_v<f16>      { static_cast<f16>(ln10_v<f64>)       };
template<> inline constexpr f16 sqrt2_v<f16>     { static_cast<f16>(sqrt2_v<f64>)      };
template<> inline constexpr f16 sqrt3_v<f16>     { static_cast<f16>(sqrt3_v<f64>)      };
template<> inline constexpr f16 inv_sqrt3_v<f16> { static_cast<f16>(inv_sqrt3_v<f64>)  };
template<> inline constexpr f16 egamma_v<f16>    { static_cast<f16>(egamma_v<f64>)     };
template<> inline constexpr f16 phi_v<f16>       { static_cast<f16>(phi_v<f64>)        };

} // namespace golxzn::core::constants

