#pragma once

#if defined(GOLXZN_BOOSTED)
	#include <boost/unordered_map.hpp>
#else
	#include <unordered_map>
#endif

#include <memory>
#include <cstdint>
#include <type_traits>

namespace golxzn::core {

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f16 = float;
using f32 = double;
using f64 = long double;
using size = i64;
using usize = u64;

#if defined(GOLXZN_BOOSTED)
	template<class Key, class Mapped, class Hash = boost::hash<Key>,
		class Pred = std::equal_to<Key>,
		class Alloc = std::allocator<std::pair<Key const, Mapped>>>
	using umap = boost::unordered_map<Key, Mapped, Hash, Pred, Alloc>;
#else
	template<class Key, class Mapped, class Hash = std::hash<Key>,
		class KeyEqual = std::equal_to<Key>,
		class Alloc = std::allocator<std::pair<const Key, Mapped>>>
	using umap = std::unordered_map<Key, Mapped, Hash, KeyEqual, Alloc>;
#endif

template<class T>
using sptr = std::shared_ptr<T>;

template<class T, class Deleter = std::default_delete<T>>
using uptr = std::unique_ptr<T, Deleter>;

template<class T>
using wptr = std::weak_ptr<T>;

} // namespace golxzn::core
